CREATE TABLE public.traffic_metrics (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP,
    up_version SMALLINT,
    up_address BYTEA,
    down_version SMALLINT,
    down_address BYTEA,
    up_port INTEGER,
    down_ports INTEGER[],
    up_size BIGINT,
    down_size BIGINT
);

