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
