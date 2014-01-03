-- adds two hstores values by converting them to integers
-- Select hstore_add('a=>1,b=>2'::hstore,'b=>1,c=>2'::inthstore)
-- => {'a'=>'1','b'=>'3','c'=>'2'}
CREATE FUNCTION hstore_add(a inthstore, b inthstore)
RETURNS inthstore AS
'$libdir/pg_numhstore.so', 'hstore_add' LANGUAGE C IMMUTABLE;

CREATE FUNCTION hstore_add(a floathstore, b floathstore) RETURNS floathstore AS $$
BEGIN
RETURN
hstore(
  array_agg(key),
  array_agg((COALESCE(r.value::numeric,0) + COALESCE(l.value::numeric,0))::text)
)
FROM each(a) l
FULL OUTER JOIN each(b) r
USING (key);
END;
$$ LANGUAGE 'plpgsql' IMMUTABLE;

CREATE FUNCTION hstore_add(a inthstore, b bigint) RETURNS inthstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::bigint + b)::text)
      ),''::inthstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE FUNCTION hstore_add(a floathstore, b numeric) RETURNS floathstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::numeric + b)::text)
      ),''::inthstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;
