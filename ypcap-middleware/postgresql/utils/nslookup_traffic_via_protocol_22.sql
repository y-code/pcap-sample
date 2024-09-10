SELECT
  inet( format( '%s.%s.%s.%s', get_byte(src, 0), get_byte(src, 1), get_byte(src, 2), get_byte(src, 3) ) ) AS src_ip,
  inet( format( '%s.%s.%s.%s', get_byte(dst, 0), get_byte(dst, 1), get_byte(dst, 2), get_byte(dst, 3) ) ) AS dst_ip
FROM (
  SELECT
    decode(src_address, 'base64') AS src,
    decode(dst_address, 'base64') AS dst
  FROM cypher('traffic_graph_22', $$
    MATCH (n:protoco_client { server_port: 22 })-[e:traffic]->(o:protocol_server { port: 22 })
    RETURN n.address, o.address
  $$) AS (src_address TEXT, dst_address TEXT)
)
ORDER BY src, dst
