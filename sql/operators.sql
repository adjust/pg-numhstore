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
