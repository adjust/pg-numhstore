-- multiplies hstore with int
-- Select hstore_div('foo=>10, bar => 15'::hstore, 5)
-- =>  "bar"=>"15", "foo"=>"50"
CREATE FUNCTION hstore_mul(a inthstore, b bigint) RETURNS inthstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::integer * b)::text)
      ),''::inthstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE FUNCTION hstore_mul(a floathstore, b numeric) RETURNS floathstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::numeric * b)::text)
      ),''::floathstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE  STRICT;


CREATE FUNCTION hstore_mul(a floathstore, b floathstore) RETURNS floathstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::decimal * r.value::decimal)::text)
      ),''::hstore)
      FROM each(a) l
      FULL OUTER JOIN each(b) r
      USING (key);
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE  STRICT;

CREATE FUNCTION hstore_mul(a inthstore, b inthstore) RETURNS floathstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::integer * r.value::integer)::text)
      ),''::hstore)
      FROM each(a) l
      FULL OUTER JOIN each(b) r
      USING (key);
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE  STRICT;
