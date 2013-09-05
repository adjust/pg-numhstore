EXTENSION = numhstore
EXTVERSION   = $(shell grep default_version $(EXTENSION).control | \
			   sed -e "s/default_version[[:space:]]*=[[:space:]]*'\([^']*\)'/\1/")
DATA         = $(filter-out $(wildcard sql/*--*.sql),$(wildcard sql/*.sql))
MODULE_big = pg_numhstore
OBJS = src/array_count.o src/avltree.o src/hstore_add.o src/array_add.o src/pg_numhstore.o
PG_CPPFLAGS += -std=c99

TESTS        = $(wildcard test/sql/*.sql)
REGRESS      = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test --load-language=plpgsql

PG_CONFIG = pg_config

all: concat

concat:
	echo > sql/$(EXTENSION)--$(EXTVERSION).sql
	cat $(filter-out $(wildcard sql/*--*.sql),$(wildcard sql/*.sql)) >> sql/$(EXTENSION)--$(EXTVERSION).sql


DATA = $(wildcard sql/*--*.sql) sql/$(EXTENSION)--$(EXTVERSION).sql
EXTRA_CLEAN = sql/$(EXTENSION)--$(EXTVERSION).sql
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
