-- sums the values of an hstore
-- SELECT hstore_sum_up('oli=>34,manuel=>25'::hstore)
-- => 59
CREATE FUNCTION hstore_sum_up(store inthstore) RETURNS bigint AS $$
BEGIN
RETURN
  SUM(value::bigint) FROM each(store);
END
$$ LANGUAGE 'plpgsql' IMMUTABLE;

CREATE FUNCTION hstore_sum_up(store floathstore) RETURNS numeric AS $$
BEGIN
  RETURN
    SUM(value::numeric) FROM each(store);
END
$$ LANGUAGE 'plpgsql' IMMUTABLE;

