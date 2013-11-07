CREATE OR REPLACE FUNCTION hstore_div(a floathstore, b floathstore) RETURNS floathstore AS $$
DECLARE
  key_match boolean;
  missing_keys text[];
BEGIN  
  SELECT akeys(a) <@ akeys(b) INTO key_match;
  IF NOT key_match THEN
    
    SELECT array_agg(l) 
    FROM  skeys(a) l
    FULL OUTER JOIN skeys(b) r ON l = r 
    WHERE r IS NULL 
    INTO missing_keys;
    
    RAISE EXCEPTION USING
      message = 'Keys of the numerator doesn''t match denominator.',
      detail  = 'extra keys are ' || missing_keys::text,
      hint    = 'You may want to slice before';
  END IF;

RETURN
  COALESCE (hstore(
    array_agg(key),
    array_agg((l.value::decimal / r.value::decimal)::text)
  ),''::hstore)
  FROM each(a) l
  RIGHT JOIN each(b) r
  USING (key);
END
$$ LANGUAGE 'plpgsql' IMMUTABLE STRICT;