
-- complain if script is sourced in psql, rather than via UPDATE EXTENSION
\echo Use "UPDATE EXTENSION numhstore" to load this file. \quit
-- counts attributes from an array and stores them as hstore object
-- e.g.
-- Select array_count(ARRAY['de','de','us'])
-- => { 'de' => '2', 'us' => '1' }

CREATE OR REPLACE FUNCTION array_count(integer[])
RETURNS inthstore AS
'$libdir/pg_numhstore.so', 'array_count' LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION array_add(text[], integer[])
RETURNS inthstore AS
'$libdir/pg_numhstore.so', 'array_add' LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION array_count(text[])
RETURNS inthstore AS
'$libdir/pg_numhstore.so', 'array_count' LANGUAGE C IMMUTABLE STRICT;
-- adds two hstores values by converting them to integers
-- Select hstore_add('a=>1,b=>2'::hstore,'b=>1,c=>2'::inthstore)
-- => {'a'=>'1','b'=>'3','c'=>'2'}
CREATE OR REPLACE FUNCTION hstore_add(a inthstore, b inthstore)
RETURNS inthstore AS
'$libdir/pg_numhstore.so', 'hstore_add' LANGUAGE C IMMUTABLE;

CREATE OR REPLACE FUNCTION hstore_add(a floathstore, b floathstore) RETURNS floathstore AS $$
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

CREATE OR REPLACE FUNCTION hstore_add(a inthstore, b bigint) RETURNS inthstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::bigint + b)::text)
      ),''::inthstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hstore_add(a floathstore, b numeric) RETURNS floathstore AS $$
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
CREATE OR REPLACE FUNCTION hstore_div(a floathstore, b numeric) RETURNS floathstore AS $$
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
CREATE OR REPLACE FUNCTION hstore_div(a floathstore, b floathstore) RETURNS floathstore AS $$
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
-- multiplies hstore with int
-- Select hstore_div('foo=>10, bar => 15'::hstore, 5)
-- =>  "bar"=>"15", "foo"=>"50"
CREATE OR REPLACE FUNCTION hstore_mul(a inthstore, b bigint) RETURNS inthstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::integer * b)::text)
      ),''::inthstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hstore_mul(a floathstore, b numeric) RETURNS floathstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::numeric * b)::text)
      ),''::floathstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE  STRICT;


CREATE OR REPLACE FUNCTION hstore_mul(a floathstore, b floathstore) RETURNS floathstore AS $$
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

CREATE OR REPLACE FUNCTION hstore_mul(a inthstore, b inthstore) RETURNS floathstore AS $$
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
CREATE OR REPLACE FUNCTION hstore_sub(a floathstore, b floathstore) RETURNS floathstore AS $$
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

CREATE OR REPLACE FUNCTION hstore_sub(a inthstore, b inthstore) RETURNS inthstore AS $$
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

CREATE OR REPLACE FUNCTION hstore_sub(a inthstore, b bigint) RETURNS inthstore AS $$
  BEGIN
    RETURN
      COALESCE (hstore(
        array_agg(key),
        array_agg((l.value::bigint - b)::text)
      ),''::inthstore)
      FROM each(a) l ;
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hstore_sub(a floathstore, b numeric) RETURNS floathstore AS $$
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
CREATE OR REPLACE FUNCTION hstore_sum_up(store inthstore) RETURNS bigint AS $$
BEGIN
RETURN
  SUM(value::bigint) FROM each(store);
END
$$ LANGUAGE 'plpgsql' IMMUTABLE;

CREATE OR REPLACE FUNCTION hstore_sum_up(store floathstore) RETURNS numeric AS $$
BEGIN
  RETURN
    SUM(value::numeric) FROM each(store);
END
$$ LANGUAGE 'plpgsql' IMMUTABLE;

CREATE OR REPLACE FUNCTION hstore_length(store hstore) RETURNS integer AS $$
  BEGIN
    RAISE WARNING 'hstore_length is depricated and will be removed in future versions';
    RETURN
      array_length(akeys(store),1);
  END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hstore_gt(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_gt is depricated and will be removed in future versions';
RETURN
  hstore_length(a) > b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hstore_ge(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_ge is depricated and will be removed in future versions';
RETURN
  hstore_length(a) >= b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hstore_lt(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_lt is depricated and will be removed in future versions';
RETURN
  hstore_length(a) < b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hstore_le(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_le is depricated and will be removed in future versions';
RETURN
  hstore_length(a) <= b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION hstore_eq(a hstore, b integer) RETURNS boolean AS $$
BEGIN
  RAISE WARNING 'hstore_eq is depricated and will be removed in future versions';
RETURN
  hstore_length(a) = b;
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;

-- returns hstore from two array where a is keys and b is integers to summarize
-- Select hstore_sum_array(Array['1','1','3'], Array[2,3,5])
-- => ""1"=>"5", "3"=>"5""
CREATE OR REPLACE FUNCTION hstore_sum_array(a text[], b integer[]) RETURNS inthstore AS $$
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