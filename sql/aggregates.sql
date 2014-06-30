--used for avg(hstore) first array holds hstore sum and count of keys
CREATE FUNCTION hstore_accum(a inthstore[], b inthstore) RETURNS inthstore[] AS $$
BEGIN
RETURN
  Array[hstore_add(a[1],b), hstore_add(a[2],array_count(akeys(b)))];
END;
$$ LANGUAGE 'plpgsql' IMMUTABLE;

CREATE FUNCTION hstore_avg(a inthstore[]) RETURNS floathstore AS $$
BEGIN
RETURN
  COALESCE (hstore(
    array_agg(key),
    array_agg((l.value::decimal / r.value::decimal)::decimal::text)
  ),''::hstore)
  FROM each(a[1]) l
  INNER JOIN each(a[2]) r
USING (key);
END;
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;


CREATE FUNCTION hstore_accum(a floathstore[], b floathstore) RETURNS floathstore[] AS $$
BEGIN
RETURN
  Array[hstore_add(a[1],b), hstore_add(a[2],array_count(akeys(b))::floathstore)];
END;
$$ LANGUAGE 'plpgsql' IMMUTABLE;

CREATE FUNCTION hstore_avg(a floathstore[]) RETURNS floathstore AS $$
BEGIN
RETURN
  COALESCE (hstore(
    array_agg(key),
    array_agg((l.value::decimal / r.value::decimal)::decimal::text)
  ),''::hstore)
  FROM each(a[1]) l
  INNER JOIN each(a[2]) r
USING (key);
END;
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE FUNCTION hstore_array_finalfn(internal)
RETURNS inthstore AS
'$libdir/pg_numhstore.so'
LANGUAGE C IMMUTABLE STRICT;

--return the average of hstores
CREATE AGGREGATE avg (
sfunc = hstore_accum,
basetype = inthstore,
stype = inthstore[2],
finalfunc = hstore_avg,
initcond = '{}'
);

CREATE AGGREGATE avg (
sfunc = hstore_accum,
basetype = floathstore,
stype = floathstore[2],
finalfunc = hstore_avg,
initcond = '{}'
);


-- the aggregation sum of hstores
--CREATE AGGREGATE sum (
--sfunc = hstore_add,
--basetype = inthstore,
--stype = inthstore,
--initcond = ''
--);

CREATE AGGREGATE sum (
sfunc = array_agg_transfn,
basetype = inthstore,
stype = internal,
finalfunc = hstore_array_finalfn
);

CREATE AGGREGATE sum (
sfunc = hstore_add,
basetype = floathstore,
stype = floathstore,
initcond = ''
);

