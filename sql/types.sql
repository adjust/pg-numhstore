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
