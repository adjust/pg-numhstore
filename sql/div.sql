-- divides hstore with numeric
-- Select hstore_div('foo=>10, bar => 15'::hstore, 5)
-- =>  "bar"=>"3", "foo"=>"2"
CREATE FUNCTION hstore_div(a floathstore, b numeric) RETURNS floathstore AS $$
BEGIN
RETURN
  COALESCE (hstore(
    array_agg(key),
    array_agg((l.value::decimal / NULLIF(b,0))::decimal::text)
  ),''::hstore)
  FROM each(a) l ;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

-- divides hstore with hstore
-- Select hstore_div('foo=>10, bar => 15'::hstore, 'foo=>5, bar => 3'::hstore)
-- =>  "bar"=>"5", "foo"=>"2"
CREATE FUNCTION hstore_div(a floathstore, b floathstore) RETURNS floathstore AS $$
DECLARE
  key_match boolean;
  missing_keys text[];
BEGIN
RETURN
  COALESCE (hstore(
    array_agg(key),
    array_agg((l.value::decimal / r.value::decimal)::text)
  ),''::hstore)
  FROM each(a) l
  FULL OUTER JOIN each(b) r
  USING (key);
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;
