CREATE OR REPLACE PROCEDURE create_graph_from_table(graph_name TEXT, table_name TEXT)
LANGUAGE plpgsql
AS $$
DECLARE
  org_search_path TEXT;
  r RECORD;
  edge_id bigint;
  count_new_endpoints INT := 0;
  count_new_protocol_servers INT := 0;
  count_new_protocol_clients INT := 0;
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

  RAISE NOTICE 'Adding a vertex for each unique version and address';

  FOR r IN

    EXECUTE format($cmd2$
      SELECT *
      FROM (
        SELECT DISTINCT version, address
        FROM (
          SELECT DISTINCT down_version AS version, down_address AS address FROM %I
          UNION
          SELECT DISTINCT up_version AS version, up_address AS address FROM %I
        )
      ) AS v
      WHERE NOT EXISTS (
        SELECT 1
        FROM (
          SELECT version, decode(address, 'base64') AS address
          FROM cypher(%L, $age$
            MATCH (v:endpoint)
            RETURN v.version, v.address
          $age$) AS (version SMALLINT, address TEXT)
        ) g
        WHERE g.version = v.version AND g.address = v.address
      )
    $cmd2$, table_name, table_name, graph_name)

  LOOP

    EXECUTE format($cmd3$
      SELECT *
      FROM cypher(%L, $age$
        CREATE (:endpoint { version: %s, address: %I })
      $age$) AS (v agtype)
    $cmd3$, graph_name, r.version, encode(r.address, 'base64'));

    count_new_endpoints := count_new_endpoints + 1;

  END LOOP;

  RAISE NOTICE 'Added % endpoints.', count_new_endpoints;

  RAISE NOTICE 'Adding a protocol server vertex for each unique version, address and port';

  FOR r IN

    EXECUTE format($cmd2$
      SELECT *
      FROM (
        SELECT DISTINCT up_version AS version, up_address AS address, up_port AS port
        FROM %I
      ) AS v
      WHERE NOT EXISTS (
        SELECT 1
        FROM (
          SELECT version, decode(address, 'base64') AS address, port
          FROM cypher(%L, $age$
            MATCH (v:protocol_server)
            RETURN v.version, v.address, v.port
          $age$) AS (version SMALLINT, address TEXT, port INT)
        ) g
        WHERE g.version = v.version AND g.address = v.address AND g.port = v.port
      )
    $cmd2$, table_name, graph_name)

  LOOP

    EXECUTE format($cmd3$
      SELECT *
      FROM cypher(%L, $age$
        CREATE (:protocol_server { version: %s, address: %I, port: %s })
      $age$) AS (v agtype)
    $cmd3$, graph_name, r.version, encode(r.address, 'base64'), r.port);

    count_new_protocol_servers := count_new_protocol_servers + 1;

  END LOOP;

  RAISE NOTICE 'Added % protocol servers.', count_new_protocol_servers;

  RAISE NOTICE 'Adding a protocol client vertex for each unique version, address and port';

  FOR r IN

    EXECUTE format($cmd2$
      SELECT g.id, v.version, v.address, v.server_port, ARRAY_AGG(v.port) AS ports
      FROM (
        SELECT DISTINCT down_version AS version, down_address AS address, up_port as server_port, unnest(down_ports) AS port
        FROM %I
      ) AS v
      LEFT JOIN (
        SELECT id, version, decode(address, 'base64') AS address, server_port, ports
        FROM cypher(%L, $age$
          MATCH (v:protocol_client)
          RETURN id(v), v.version, v.address, v.server_port, v.ports
        $age$) AS (id BIGINT, version SMALLINT, address TEXT, server_port INTEGER, ports INTEGER[])
      ) g
      ON g.version = v.version AND g.address = v.address AND v.port = ANY(g.ports)
      GROUP BY 1, 2, 3, 4
    $cmd2$, table_name, graph_name)

  LOOP

    IF r.id IS NULL THEN

      EXECUTE format($cmd3$
        SELECT *
        FROM cypher(%L, $age$
          CREATE (:protocol_client { version: %s, address: %I, server_port: %s, ports: %s })
        $age$) AS (v agtype)
      $cmd3$, graph_name, r.version, encode(r.address, 'base64'), r.server_port, to_json(r.ports));

    ELSE

      EXECUTE format($cmd35$
        SELECT *
        FROM cypher(%L, $age$
          MATCH (v:protocol_client)
          WHERE id(v) = %I
          SET v.ports = coalesce(v.ports, []) || %s
        $age$) AS (v agtype)
      $cmd35$, graph_name, r.id, to_json(r.ports));

    END IF;

    count_new_protocol_clients := count_new_protocol_clients + 1;

  END LOOP;

  RAISE NOTICE 'Added/updated % protocol clients.', count_new_protocol_clients;

  FOR r IN

    EXECUTE format($cmd4$
      SELECT DISTINCT down_version AS version, down_address AS address, up_port AS port FROM %I
    $cmd4$, table_name)

  LOOP

    EXECUTE format($cmd5$
      SELECT *
      FROM cypher(%L, $age$
        MATCH (m:endpoint { version: %s, address: %I })-[e:endpoint_protocol]->(n:protocol_client { version: %s, address: %I, server_port: %s })
        RETURN id(e)
      $age$) AS (edge_id BIGINT)
    $cmd5$, graph_name, r.version, encode(r.address, 'base64'), r.version, encode(r.address, 'base64'), r.port)
    INTO edge_id;

    IF edge_id IS NULL THEN

      EXECUTE format($cmd6$
        SELECT *
        FROM cypher(%L, $age$
          MATCH (m:endpoint { version: %s, address: %I }), (n:protocol_client { version: %s, address: %I, server_port: %s })
          CREATE (m)-[e:endpoint_protocol]->(n)
          RETURN id(e)
        $age$) AS (edge_id BIGINT)
      $cmd6$, graph_name, r.version, encode(r.address, 'base64'), r.version, encode(r.address, 'base64'), r.port)
      INTO edge_id;

      IF edge_id IS NULL THEN
        RAISE EXCEPTION 'endpoint_protocol-type edge was not found unexpectedly (version: %s, address: %I, up_port: %s)',
          r.version, encode(r.address, 'base64'), r.up_port
        USING ERRCODE = 'P0001';
      END IF;

    END IF;

  END LOOP;

  FOR r IN

    EXECUTE format($cmd4$
      SELECT up_version AS version, up_address AS address, up_port AS port FROM %I
    $cmd4$, table_name, table_name)

  LOOP

    EXECUTE format($cmd5$
      SELECT *
      FROM cypher(%L, $age$
        MATCH (m:endpoint { version: %s, address: %I })-[e:endpoint_protocol]->(n:protocol_server { version: %s, address: %I, port: %s })
        RETURN id(e)
      $age$) AS (edge_id BIGINT)
    $cmd5$, graph_name, r.version, encode(r.address, 'base64'), r.version, encode(r.address, 'base64'), r.port)
    INTO edge_id;

    IF edge_id IS NULL THEN

      EXECUTE format($cmd6$
        SELECT *
        FROM cypher(%L, $age$
          MATCH (m:endpoint { version: %s, address: %I }), (n:protocol_server { version: %s, address: %I, port: %s })
          CREATE (m)-[e:endpoint_protocol]->(n)
          RETURN id(e)
        $age$) AS (edge_id BIGINT)
      $cmd6$, graph_name, r.version, encode(r.address, 'base64'), r.version, encode(r.address, 'base64'), r.port)
      INTO edge_id;

      IF edge_id IS NULL THEN
        RAISE EXCEPTION 'endpoint_protocol-type edge was not found unexpectedly (version: %s, address: %I, up_port: %s)',
          r.version, encode(r.address, 'base64'), r.up_port
        USING ERRCODE = 'P0001';
      END IF;

    END IF;

  END LOOP;

  FOR r IN

    EXECUTE format($cmd4$
      SELECT down_version, down_address, up_version, up_address, up_port, up_size, down_size
      FROM %I
    $cmd4$, table_name)

  LOOP

    EXECUTE format($cmd5$
      SELECT *
      FROM cypher(%L, $age$
        MATCH (m:protocol_client { version: %s, address: %I, server_port: %s })-[e:traffic]->(n:protocol_server { version: %s, address: %I, port: %s })
        RETURN id(e)
      $age$) AS (edge_id BIGINT)
    $cmd5$, graph_name, r.down_version, encode(r.down_address, 'base64'), r.up_port, r.up_version, encode(r.up_address, 'base64'), r.up_port)
    INTO edge_id;

    IF edge_id IS NULL THEN

      EXECUTE format($cmd6$
        SELECT *
        FROM cypher(%L, $age$
          MATCH (m:protocol_client { version: %s, address: %I, server_port: %s }), (n:protocol_server { version: %s, address: %I, port: %s })
          CREATE (m)-[e:traffic { up_size: 0, down_size: 0 }]->(n)
          RETURN id(e)
        $age$) AS (edge_id BIGINT)
      $cmd6$, graph_name, r.down_version, encode(r.down_address, 'base64'), r.up_port, r.up_version, encode(r.up_address, 'base64'), r.up_port)
      INTO edge_id;

    END IF;

    EXECUTE format($cmd7$
      SELECT *
      FROM cypher(%L, $age$
        MATCH ()-[e:traffic]->()
        WHERE id(e) = %s
        SET e.up_size = e.up_size + %s, e.down_size = e.down_size + %s
        RETURN id(e)
      $age$) AS (edge_id BIGINT)
    $cmd7$, graph_name, edge_id, r.up_size, r.down_size)
    INTO edge_id;

    IF edge_id IS NULL THEN
      RAISE EXCEPTION 'traffic-type edge was not found unexpectedly (down_version: %s, down_address: %I, up_version: %s, up_address: %I, up_port: %s)',
        traffic.down_version, encode(traffic.down_address, 'base64'), traffic.up_version, encode(traffic.up_address, 'base64'), traffic.up_port
      USING ERRCODE = 'P0001';
    END IF;

  END LOOP;

  EXECUTE format($cmd8$
    CREATE OR REPLACE VIEW %I.v_endpoint AS
    SELECT id, version, decode(address, 'base64') AS address
    FROM cypher(%L, $age$
      MATCH (n:endpoint)
      RETURN id(n), n.version, n.address
    $age$) AS (id BIGINT, version SMALLINT, address TEXT)
  $cmd8$, graph_name, graph_name);

  EXECUTE format($cmd8$
    CREATE OR REPLACE VIEW %I.v_protocol_server AS
    SELECT id, version, decode(address, 'base64') AS address, port
    FROM cypher(%L, $age$
      MATCH (n:protocol_server)
      RETURN id(n), n.version, n.address, n.port
    $age$) AS (id BIGINT, version SMALLINT, address TEXT, port INT)
  $cmd8$, graph_name, graph_name);

  EXECUTE format($cmd8$
    CREATE OR REPLACE VIEW %I.v_protocol_client AS
    SELECT id, version, decode(address, 'base64') AS address, server_port, ports
    FROM cypher(%L, $age$
      MATCH (n:protocol_client)
      RETURN id(n), n.version, n.address, n.server_port, n.ports
    $age$) AS (id BIGINT, version SMALLINT, address TEXT, server_port INT, ports INT[])
  $cmd8$, graph_name, graph_name);

  EXECUTE format($cmd9$
    CREATE OR REPLACE VIEW %I.v_endpoint_protocol_server AS
    SELECT *
    FROM cypher(%L, $age$
      MATCH (n:endpoint)-[e:endpoint_protocol]->(o:protocol_server)
      RETURN id(e), id(n), id(o)
    $age$) AS (id BIGINT, start BIGINT, "end" BIGINT)
  $cmd9$, graph_name, graph_name);

  EXECUTE format($cmd9$
    CREATE OR REPLACE VIEW %I.v_endpoint_protocol_client AS
    SELECT *
    FROM cypher(%L, $age$
      MATCH (n:endpoint)-[e:endpoint_protocol]->(o:protocol_client)
      RETURN id(e), id(n), id(o)
    $age$) AS (id BIGINT, start BIGINT, "end" BIGINT)
  $cmd9$, graph_name, graph_name);

  EXECUTE format($cmd9$
    CREATE OR REPLACE VIEW %I.v_traffic AS
    SELECT *
    FROM cypher(%L, $age$
      MATCH (n:protocol_client)-[e:traffic]->(o:protocol_server)
      RETURN id(e), id(n), id(o), e.up_size, e.down_size
    $age$) AS (id BIGINT, start BIGINT, "end" BIGINT, up_size INT, down_size INT)
  $cmd9$, graph_name, graph_name);

  EXECUTE 'SET search_path TO '||org_search_path;

END;
$$;

