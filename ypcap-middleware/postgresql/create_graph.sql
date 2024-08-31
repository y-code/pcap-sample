CREATE OR REPLACE PROCEDURE create_graph_from_table(graph_name TEXT, table_name TEXT)
LANGUAGE plpgsql
AS $$
DECLARE
  org_search_path TEXT;
  node RECORD;
  traffic RECORD;
  edge_id bigint;
  cnt_new_p_address INT := 0;
BEGIN
  RAISE NOTICE 'Creating the graph if it doesn''t exist';

  SELECT current_setting('search_path') INTO org_search_path;
  EXECUTE 'SET search_path TO ag_catalog, '||org_search_path;

  IF NOT EXISTS (
    SELECT 1
    FROM ag_catalog.ag_graph
    WHERE name = graph_name
  ) THEN

    EXECUTE format($cmd1$
      SELECT create_graph(%L)
    $cmd1$, graph_name);

  END IF;

  RAISE NOTICE 'Adding vertices for each unique src_version and dst_address';

  FOR node IN

    EXECUTE format($cmd2$
      SELECT *
      FROM (
        SELECT DISTINCT version, ip, port
        FROM (
          SELECT DISTINCT src_version AS version, src_address AS ip, port FROM %I
          UNION
          SELECT DISTINCT dst_version AS version, dst_address AS ip, port FROM %I
        )
      ) AS nodes
      WHERE NOT EXISTS (
        SELECT 1
        FROM (
          SELECT version, decode(ip, 'base64') AS ip, port
          FROM cypher(%L, $age$
            MATCH (v:p_endpoint)
            RETURN v.version, v.ip, v.port
          $age$) AS (version SMALLINT, ip TEXT, port INT)
        ) g
        WHERE g.version = nodes.version AND g.ip = nodes.ip AND g.port = nodes.port
      )
    $cmd2$, table_name, table_name, graph_name)

  LOOP

    EXECUTE format($cmd3$
      SELECT *
      FROM cypher(%L, $age$
        CREATE (:p_endpoint { version: %s, ip: %I, port: %s })
      $age$) AS (v agtype)
    $cmd3$, graph_name, node.version, encode(node.ip, 'base64'), node.port);

    cnt_new_p_address := cnt_new_p_address + 1;

  END LOOP;

  RAISE NOTICE 'Added % protocol addresses.', cnt_new_p_address ;

  FOR traffic IN

    EXECUTE format($cmd4$
      SELECT src_version, src_address, dst_version, dst_address, port, count, size
      FROM %I
    $cmd4$, table_name)

  LOOP

    EXECUTE format($cmd5$
      SELECT *
      FROM cypher(%L, $age$
        MATCH (m:p_endpoint { version: %s, ip: %I, port: %s })-[e:p_traffic]->(n:p_endpoint { version: %s, ip: %I, port: %s })
        RETURN id(e)
      $age$) AS (edge_id BIGINT)
    $cmd5$, graph_name, traffic.src_version, encode(traffic.src_address, 'base64'), traffic.port, traffic.dst_version, encode(traffic.dst_address, 'base64'), traffic.port)
    INTO edge_id;

    IF edge_id IS NULL THEN

      EXECUTE format($cmd6$
        SELECT *
        FROM cypher(%L, $age$
          MATCH (m:p_endpoint { version: %s, ip: %I, port: %s }), (n:p_endpoint { version: %s, ip: %I, port: %s })
          CREATE (m)-[e:p_traffic { count: 0, size: 0 }]->(n)
          RETURN id(e)
        $age$) AS (edge_id BIGINT)
      $cmd6$, graph_name, traffic.src_version, encode(traffic.src_address, 'base64'), traffic.port, traffic.dst_version, encode(traffic.dst_address, 'base64'), traffic.port)
      INTO edge_id;

    END IF;

    EXECUTE format($cmd7$
      SELECT *
      FROM cypher(%L, $age$
        MATCH ()-[e:p_traffic]->()
        WHERE id(e) = %s
        SET e.count = e.count + %s, e.size = e.size + %s
        RETURN id(e)
      $age$) AS (edge_id BIGINT)
    $cmd7$, graph_name, edge_id, traffic.count, traffic.size)
    INTO edge_id;

    IF edge_id IS NULL THEN
      RAISE EXCEPTION 'traffic-type edge was not found unexpectedly (src_version: %s, src_address: %I, dst_version: %s, dst_address: %I, port: %s)',
        traffic.src_version, encode(traffic.src_address, 'base64'), traffic.dst_version, encode(traffic.dst_address, 'base64'), traffic.port
      USING ERRCODE = 'P0001';
    END IF;

  END LOOP;

  EXECUTE format($cmd8$
    CREATE OR REPLACE VIEW %I.protocol_endpoint AS
    SELECT id, version, decode(ip, 'base64') AS address, port
    FROM cypher(%L, $age$
      MATCH (n:p_endpoint)
      RETURN id(n), n.version, n.ip, n.port
    $age$) AS (id BIGINT, version SMALLINT, ip TEXT, port INT)
  $cmd8$, graph_name, graph_name);

  EXECUTE format($cmd9$
    CREATE OR REPLACE VIEW %I.protocol_traffic AS
    SELECT *
    FROM cypher(%L, $age$
      MATCH (n:p_endpoint)-[e:p_traffic]->(o:p_endpoint)
      RETURN id(n), id(o), e.count, e.size
    $age$) AS (start BIGINT, "end" BIGINT, count INT, size INT)
  $cmd9$, graph_name, graph_name);

  EXECUTE 'SET search_path TO '||org_search_path;

END;
$$;

