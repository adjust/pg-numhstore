SELECT array_count(ARRAY['de','de','us']);
SELECT array_count(ARRAY[1,5,9,5,10,5,5]);
SELECT hstore_add('a=>1,b=>2'::hstore,'b=>1,c=>2'::inthstore);
SELECT hstore_add('a=>1,b=>1000000000'::hstore,'b=>1,c=>2'::inthstore);
SELECT hstore_add('a=>1.3,b=>2.5'::hstore,'b=>1.2,c=>2.4'::floathstore);
SELECT hstore_add('a=>1,b=>2'::hstore,'b=>1,c=>2'::inthstore);
SELECT hstore_sub('a=>2,b=>3'::hstore,'b=>1,c=>2'::inthstore);
SELECT array_uniq(ARRAY[5,4,13,4,5,2]);

SELECT tracker_id, install_date, device_id, 
array_count( array_agg( CASE WHEN Date(created_at) - install_date  <= 30 THEN Date(created_at) - install_date ELSE NULL END ) ) as daily_session_cohort
FROM 
(
  SELECT 1, i, generate_series(('2012-01-01'::date +i)::timestamp,('2012-01-01'::date +i+100)::timestamp - '1 hour'::interval, '1 hour'), '2012-01-01'::date
	FROM  generate_series(1,100) i)sessions(tracker_id,device_id, created_at, install_date
)
WHERE device_id=30
GROUP BY tracker_id, install_date, device_id;

SELECT tracker_id, install_date, device_id, 
array_count( array_agg( CASE WHEN Date(created_at) - install_date  <= 30 THEN Date(created_at) - install_date ELSE NULL END )::text[] ) as daily_session_cohort
FROM 
(
  SELECT 1, i, generate_series(('2012-01-01'::date +i)::timestamp,('2012-01-01'::date +i+100)::timestamp - '1 hour'::interval, '1 hour'), '2012-01-01'::date
	FROM  generate_series(1,100) i)sessions(tracker_id,device_id, created_at, install_date
)
WHERE device_id=30
GROUP BY tracker_id, install_date, device_id;
