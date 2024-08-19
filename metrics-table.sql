CREATE TABLE traffic_metrics (
    id SERIAL PRIMARY KEY,
    src_version smallint,
    src_address BYTEA,
    dst_version smallint,
    dst_address BYTEA,
    port integer,
    count bigint,
    size bigint
);

