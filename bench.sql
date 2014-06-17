DROP DATABASE test2;
CREATE DATABASE test2;
\c test2
\timing on

CREATE FUNCTION random_string(length integer) RETURNS text AS $$
  BEGIN
    RETURN
      array_to_string(array((
        SELECT SUBSTRING('abcdefghjklmnpqrstuvwxyz23456789'
                    FROM floor(random()*32)::int+1 FOR 1)
        FROM generate_series(1,length))),'');
  END;
$$ LANGUAGE 'plpgsql';


--- creating 300 random keys of length 10
CREATE TABLE keys AS
SELECT random_string(10)  as key FROM generate_series(1, 300);

-- create input data
-- for a date span of 300 days for each key between 10 and 1000 keys
CREATE TABLE input AS
SELECT day, key FROM(
SELECT
  '2014-01-01'::date + d as day,
  key,
  generate_series(1,(random()*1000 +10)::int)

FROM generate_series(1, 300) d,
keys
) t;

CREATE INDEX input_pkey ON input USING(day,key);

CREATE EXTENSION hstore;
CREATE EXTENSION numhstore;

CREATE TABLE hstore_data AS
SELECT day, array_count(array_agg(key)) as dist FROM input GROUP BY 1;

