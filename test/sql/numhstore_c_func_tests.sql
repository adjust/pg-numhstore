SELECT array_count(ARRAY['de','de','us']);
SELECT array_count(ARRAY[1,5,9,5,10,5,5]);
SELECT hstore_add('a=>1,b=>2'::hstore,'b=>1,c=>2'::inthstore);
SELECT hstore_add('a=>1,b=>1000000000'::hstore,'b=>1,c=>2'::inthstore);
SELECT hstore_add('a=>1.3,b=>2.5'::hstore,'b=>1.2,c=>2.4'::floathstore);
SELECT hstore_add('a=>1,b=>2'::hstore,'b=>1,c=>2'::inthstore);
SELECT hstore_sub('a=>2,b=>3'::hstore,'b=>1,c=>2'::inthstore);
