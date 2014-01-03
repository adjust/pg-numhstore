CREATE FUNCTION hstore_sub(a floathstore, b floathstore) RETURNS floathstore AS $$
BEGIN
RETURN
hstore(
  array_agg(key),
  array_agg((COALESCE(l.value::numeric,0) - COALESCE(r.value::numeric,0))::text)
)
FROM each(a) l
FULL OUTER JOIN each(b) r
USING (key);
END;
$$ LANGUAGE 'plpgsql' IMMUTABLE;

CREATE FUNCTION hstore_sub(a inthstore, b inthstore) RETURNS inthstore AS $$
BEGIN
RETURN
hstore(
  array_agg(key),
  array_agg((COALESCE(l.value::bigint,0) - COALESCE(r.value::bigint,0))::text)
)
FROM each(a) l
FULL OUTER JOIN each(b) r
USING (key);
END;
$$ LANGUAGE 'plpgsql' IMMUTABLE;

CREATE FUNCTION hstore_sub(a inthstore, b bigint) RETURNS inthstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::bigint - b)::text)
      ),''::inthstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE FUNCTION hstore_sub(a floathstore, b numeric) RETURNS floathstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::numeric - b)::text)
      ),''::inthstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;
