EXTENSION = numhstore
EXTVERSION   = $(shell grep default_version $(EXTENSION).control | \
			   sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")
DATA         = $(filter-out $(wildcard sql/*--*.sql),$(wildcard sql/*.sql))
MODULE_big = pg_numhstore
OBJS = src/hstore_array.o src/array_count.o src/data_types.o src/hstore_add.o src/array_add.o src/pg_numhstore.o

TESTS        = setup $(filter-out test/sql/setup.sql test/sql/update.sql, $(wildcard test/sql/*.sql)) \
							 update $(filter-out test/sql/setup.sql test/sql/update.sql, $(wildcard test/sql/*.sql))
REGRESS      = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --load-language=plpgsql

PG_CONFIG = pg_config

all:

release:
	echo > sql/$(EXTENSION)--$(EXTVERSION).sql
	#cat $(filter-out $(wildcard sql/*--*.sql),$(wildcard sql/*.sql))
	cat \
		sql/numhstore.sql \
		sql/types.sql \
		sql/hstore_from_array.sql \
		sql/add.sql \
	 	sql/div.sql \
		sql/mul.sql \
		sql/sub.sql \
		sql/helper.sql \
		sql/aggregates.sql \
		sql/operators.sql \
		sql/depricated.sql \
	>> sql/$(EXTENSION)--$(EXTVERSION).sql

FUNCTIONS = \
	sql/hstore_from_array.sql \
	sql/add.sql \
	sql/div.sql \
	sql/mul.sql \
	sql/sub.sql \
	sql/helper.sql \
	sql/depricated.sql

new_version:

	echo > sql/$(EXTENSION)--$(EXTVERSION)--$(NEWVERSION).sql

	sed 's/CREATE EXTENSION/UPDATE EXTENSION/g' sql/numhstore.sql >> sql/$(EXTENSION)--$(EXTVERSION)--$(NEWVERSION).sql

	for i in $(FUNCTIONS); \
		do sed 's/CREATE FUNCTION/CREATE OR REPLACE FUNCTION/g' $$i >> sql/$(EXTENSION)--$(EXTVERSION)--$(NEWVERSION).sql;\
	done

	sed -i '' 's/$(EXTVERSION)/$(NEWVERSION)/g' numhstore.control

DATA = $(wildcard sql/*--*.sql)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
