CREATE TABLE public.traffic_metrics (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP,
    src_version SMALLINT,
    src_address BYTEA,
    dst_version SMALLINT,
    dst_address BYTEA,
    port INTEGER,
    count BIGINT,
    size BIGINT
);

