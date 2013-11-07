SELECT 'a=>3, b=>2'::inthstore + 'a=>2, c=>5'::inthstore;
SELECT 'a=>3, b=>2'::inthstore - 'a=>2, c=>5'::inthstore;
SELECT 'a=>3, b=>2'::inthstore * 'a=>2, c=>5'::inthstore;
SELECT 'a=>3, b=>2'::inthstore / 'a=>2, b=>5'::inthstore;
SELECT 'a=>3, b=>2'::inthstore / 'a=>2, b=>2, c=>5'::inthstore;
SELECT 'a=>3, b=>2, c=>5'::inthstore / 'a=>2, b=>2'::inthstore;

SELECT 'a=>3, b=>2'::floathstore + 'a=>2, c=>5'::floathstore;
SELECT 'a=>3, b=>2'::floathstore - 'a=>2, c=>5'::floathstore;
SELECT 'a=>3, b=>2'::floathstore * 'a=>2, c=>5'::floathstore;
SELECT 'a=>3, b=>2'::floathstore / 'a=>2, b=>5'::floathstore;
SELECT 'a=>3, b=>2'::floathstore / 'a=>2, b=>2, c=>5'::floathstore;
SELECT 'a=>3, b=>2, c=>5'::floathstore / 'a=>2, b=>2'::floathstore;

SELECT 'a=>3, b=>2'::floathstore + 'a=>2, c=>5'::inthstore;
SELECT 'a=>3, b=>2'::floathstore - 'a=>2, c=>5'::inthstore;
SELECT 'a=>3, b=>2'::floathstore * 'a=>2, c=>5'::inthstore;
SELECT 'a=>3, b=>2'::floathstore / 'a=>2, b=>5'::inthstore;
SELECT 'a=>3, b=>2'::floathstore / 'a=>2, b=>2, c=>5'::inthstore;
SELECT 'a=>3, b=>2, c=>5'::floathstore / 'a=>2, b=>2'::inthstore;

SELECT 'a=>3, b=>2'::inthstore + 'a=>2, c=>5'::floathstore;
SELECT 'a=>3, b=>2'::inthstore - 'a=>2, c=>5'::floathstore;
SELECT 'a=>3, b=>2'::inthstore * 'a=>2, c=>5'::floathstore;
SELECT 'a=>3, b=>2'::inthstore / 'a=>2, b=>5'::floathstore;
SELECT 'a=>3, b=>2'::inthstore / 'a=>2, b=>2, c=>5'::floathstore;
SELECT 'a=>3, b=>2, c=>5'::inthstore / 'a=>2, b=>2'::floathstore;


SELECT 'a=>3, b=>2'::inthstore + 3;
SELECT 'a=>3, b=>2'::inthstore - 3;
SELECT 'a=>3, b=>2'::inthstore * 3;
SELECT 'a=>3, b=>2'::inthstore / 3;

SELECT 'a=>3, b=>2'::floathstore + 3;
SELECT 'a=>3, b=>2'::floathstore - 3;
SELECT 'a=>3, b=>2'::floathstore * 3;
SELECT 'a=>3, b=>2'::floathstore / 3;