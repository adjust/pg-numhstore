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

