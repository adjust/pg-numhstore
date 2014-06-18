DROP AGGREGATE sum(inthstore);
CREATE AGGREGATE sum (
sfunc = hstore_add,
basetype = inthstore,
stype = inthstore,
initcond = ''
);
