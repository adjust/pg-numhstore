
-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION numhstore" to load this file. \quit
--additional types for numhstore

CREATE TYPE inthstore;

CREATE FUNCTION hstore_out(inthstore)
RETURNS cstring AS
'$libdir/hstore', 'hstore_out'
LANGUAGE c IMMUTABLE STRICT
COST 1;

CREATE FUNCTION hstore_send(inthstore)
RETURNS bytea AS
'$libdir/hstore', 'hstore_send'
LANGUAGE c IMMUTABLE STRICT
COST 1;

CREATE FUNCTION inthstore_in(cstring)
RETURNS inthstore AS
'$libdir/hstore', 'hstore_in'
LANGUAGE c IMMUTABLE STRICT
COST 1;

CREATE FUNCTION inthstore_recv(internal)
RETURNS inthstore AS
'$libdir/hstore', 'hstore_recv'
LANGUAGE c IMMUTABLE STRICT
COST 1;

CREATE TYPE inthstore (
      INTERNALLENGTH = -1,
      INPUT = inthstore_in,
      OUTPUT = hstore_out,
      RECEIVE = inthstore_recv,
      SEND = hstore_send,
      STORAGE = extended
);
CREATE CAST (inthstore AS hstore) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (hstore AS inthstore) WITHOUT FUNCTION AS IMPLICIT;

CREATE TYPE floathstore;

CREATE FUNCTION hstore_out(floathstore)
RETURNS cstring AS
'$libdir/hstore', 'hstore_out'
LANGUAGE c IMMUTABLE STRICT
COST 1;

CREATE FUNCTION hstore_send(floathstore)
RETURNS bytea AS
'$libdir/hstore', 'hstore_send'
LANGUAGE c IMMUTABLE STRICT
COST 1;

CREATE FUNCTION floathstore_in(cstring)
RETURNS floathstore AS
'$libdir/hstore', 'hstore_in'
LANGUAGE c IMMUTABLE STRICT
COST 1;

CREATE FUNCTION floathstore_recv(internal)
RETURNS floathstore AS
'$libdir/hstore', 'hstore_recv'
LANGUAGE c IMMUTABLE STRICT
COST 1;

CREATE TYPE floathstore (
      INTERNALLENGTH = -1,
      INPUT = floathstore_in,
      OUTPUT = hstore_out,
      RECEIVE = floathstore_recv,
      SEND = hstore_send,
      STORAGE = extended
);


CREATE CAST (floathstore AS hstore) WITHOUT FUNCTION AS IMPLICIT;
CREATE CAST (hstore AS floathstore) WITHOUT FUNCTION AS IMPLICIT;

CREATE CAST (inthstore AS floathstore) WITHOUT FUNCTION AS IMPLICIT;
-- counts attributes from an array and stores them as hstore object
-- e.g.
-- Select array_count(ARRAY['de','de','us'])
-- => { 'de' => '2', 'us' => '1' }

CREATE FUNCTION array_count(integer[])
RETURNS inthstore AS
'$libdir/pg_numhstore.so', 'array_count' LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION array_add(text[], integer[])
RETURNS inthstore AS
'$libdir/pg_numhstore.so', 'array_add' LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION array_count(text[])
RETURNS inthstore AS
'$libdir/pg_numhstore.so', 'array_count' LANGUAGE C IMMUTABLE STRICT;
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
  SELECT akeys(a) <@ akeys(b) INTO key_match;
  IF NOT key_match THEN

    SELECT array_agg(l)
    FROM  skeys(a) l
    FULL OUTER JOIN skeys(b) r ON l = r
    WHERE r IS NULL
    INTO missing_keys;

    RAISE EXCEPTION USING
      message = 'Keys of the numerator doesn''t match denominator.',
      detail  = 'extra keys are ' || missing_keys::text,
      hint    = 'You may want to slice before';
  END IF;

RETURN
  COALESCE (hstore(
    array_agg(key),
    array_agg((l.value::decimal / r.value::decimal)::text)
  ),''::hstore)
  FROM each(a) l
  RIGHT JOIN each(b) r
  USING (key);
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;
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
CREATE AGGREGATE sum (
sfunc = hstore_add,
basetype = inthstore,
stype = inthstore,
initcond = ''
);

CREATE AGGREGATE sum (
sfunc = hstore_add,
basetype = floathstore,
stype = floathstore,
initcond = ''
);
-- the / operator for the hstore_div_hstore
CREATE OPERATOR / (
    leftarg = floathstore,
    rightarg = floathstore,
    procedure = hstore_div
);

-- the / operator for the hstore div
CREATE OPERATOR / (
    leftarg = floathstore,
    rightarg = numeric,
    procedure = hstore_div
);


-- the / operator for the hstore div
CREATE OPERATOR * (
leftarg = inthstore,
rightarg = bigint,
procedure = hstore_mul
);

CREATE OPERATOR * (
leftarg = floathstore,
rightarg = numeric,
procedure = hstore_mul
);

CREATE OPERATOR + (
leftarg = inthstore,
rightarg = bigint,
procedure = hstore_add
);

CREATE OPERATOR + (
leftarg = floathstore,
rightarg = numeric,
procedure = hstore_add
);

CREATE OPERATOR - (
leftarg = inthstore,
rightarg = bigint,
procedure = hstore_sub
);

CREATE OPERATOR - (
leftarg = floathstore,
rightarg = numeric,
procedure = hstore_sub
);

CREATE OPERATOR * (
leftarg = inthstore,
rightarg = inthstore,
procedure = hstore_mul
);

CREATE OPERATOR * (
leftarg = floathstore,
rightarg = floathstore,
procedure = hstore_mul
);


-- the - operator for the hstore div
CREATE OPERATOR - (
leftarg = inthstore,
rightarg = inthstore,
procedure = hstore_sub
);

CREATE OPERATOR - (
leftarg = floathstore,
rightarg = floathstore,
procedure = hstore_sub
);

CREATE OPERATOR + (
leftarg = inthstore,
rightarg = inthstore,
procedure = hstore_add
);

CREATE OPERATOR + (
leftarg = floathstore,
rightarg = floathstore,
procedure = hstore_add
);
CREATE FUNCTION hstore_length(store hstore) RETURNS integer AS $$
  BEGIN
    RAISE WARNING 'hstore_length is depricated and will be removed in future versions';
    RETURN
      array_length(akeys(store),1);
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE FUNCTION hstore_gt(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_gt is depricated and will be removed in future versions';
RETURN
  hstore_length(a) > b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE FUNCTION hstore_ge(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_ge is depricated and will be removed in future versions';
RETURN
  hstore_length(a) >= b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE FUNCTION hstore_lt(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_lt is depricated and will be removed in future versions';
RETURN
  hstore_length(a) < b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE FUNCTION hstore_le(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_le is depricated and will be removed in future versions';
RETURN
  hstore_length(a) <= b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE FUNCTION hstore_eq(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_eq is depricated and will be removed in future versions';
RETURN
  hstore_length(a) = b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

-- returns hstore from two array where a is keys and b is integers to summarize
-- Select hstore_sum_array(Array['1','1','3'], Array[2,3,5])
-- => ""1"=>"5", "3"=>"5""
CREATE FUNCTION hstore_sum_array(a text[], b integer[]) RETURNS inthstore AS $$
BEGIN
RAISE WARNING USING
      message = 'hstore_sum_array is depricated and will be removed in future versions',
      hint    = 'use array_add instead';
RETURN
hstore(array_agg(key),array_agg(sum)) FROM
(
Select key,SUM(val)::text FROM (Select unnest(a) as key , unnest(b) as val) t
GROUP BY key
)t;
END
$$ LANGUAGE 'plpgsql' ;


CREATE OPERATOR > (
leftarg = hstore,
rightarg = integer,
procedure = hstore_gt
);

CREATE OPERATOR >= (
leftarg = hstore,
rightarg = integer,
procedure = hstore_ge
);

CREATE OPERATOR < (
leftarg = hstore,
rightarg = integer,
procedure = hstore_lt
);

CREATE OPERATOR <= (
leftarg = hstore,
rightarg = integer,
procedure = hstore_le
);

CREATE OPERATOR = (
leftarg = hstore,
rightarg = integer,
procedure = hstore_eq
);

