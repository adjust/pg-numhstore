[![Build Status](https://travis-ci.org/adeven/pg-numhstore.png?branch=master)](https://travis-ci.org/adeven/pg-numhstore)

# Overview

This Postgres extension introduces two new data types `inthstore` and `floathstore` wich depends on postgres
[hstore](http://www.postgresql.org/docs/9.2/static/hstore.html) extension.
These types are used to hold integer and float values respectively.
`numhstore` comes with certain aggregate and helper functions.

##Operators

### `inthstore + inthstore`

```postgres
SELECT 'a=>3, b=>2'::inthstore + 'a=>2, c=>5'::inthstore;
           inthstore
------------------------------
 "a"=>"5", "b"=>"2", "c"=>"5"
```

### `inthstore - inthstore`

```postgres
SELECT 'a=>3, b=>2'::inthstore - 'a=>2, c=>5'::inthstore;
           inthstore
-------------------------------
 "a"=>"1", "b"=>"2", "c"=>"-5"
 ```

### `inthstore * inthstore`

```postgres
SELECT 'a=>3, b=>2'::inthstore * 'a=>2, c=>5'::inthstore;
            inthstore
--------------------------------
 "a"=>"6", "b"=>NULL, "c"=>NULL

 ```

### `inthstore / inthstore`

```postgres
SELECT 'a=>3, b=>2'::inthstore / 'a=>2, c=>5'::inthstore;
                        floathstore
----------------------------------------------------------
 "a"=>"1.5000000000000000", "c"=>"0.00000000000000000000"
 ```

 ### `inthstore + int`

```postgres
SELECT 'a=>3, b=>2'::inthstore + 3;
      inthstore
--------------------
 "a"=>"6", "b"=>"5"
 ```

 ### `inthstore - int`

```postgres
SELECT 'a=>3, b=>2'::inthstore - 3;
      inthstore
---------------------
 "a"=>"0", "b"=>"-1"
 ```

 ### `inthstore * int`

```postgres
SELECT 'a=>3, b=>2'::inthstore * 3;
      inthstore
--------------------
 "a"=>"9", "b"=>"6"
 ```


 ### `inthstore / int`

```postgres
SELECT 'a=>3, b=>2'::inthstore / 3;
                           floathstore
--------------------------------------------------------------
 "a"=>"1.00000000000000000000", "b"=>"0.66666666666666666667"
 ```

##Functions

## `array_count`

```postgres
SELECT array_count(ARRAY['a','a','b']);
     inthstore
----------------------
 "a"=>"2", "b"=>"1"
```

## `hstore_sum_up`

```postgres
SELECT hstore_sum_up('foo=>10, bar => 15'::inthstore);
 bigint
---------
      25
```

```postgres
SELECT hstore_sum_up('foo=>2.5, bar => 4.5'::floathstore);
  numeric
----------
      7.0
```

## `array_add`

```postgres
SELECT array_add(Array['foo','foo','bar'], Array[2,3,5]);
       inthstore
------------------------
 "bar"=>"5", "foo"=>"5"
```


## Aggregations

###SUM

```postgres
SELECT SUM(store) FROM (VALUES('a=>2, b=>4'),('a=>4, b=>6, c=>6'::inthstore) )t(store);
              inthstore
-------------------------------
 "a"=>"6", "b"=>"10", "c"=>"6"
```

###AVG

```postgres
SELECT AVG(store) FROM (VALUES('a=>2, b=>4'),('a=>4, b=>6, c=>6'::inthstore) )t(store);
                                       floathstore
---------------------------------------------------------------------------------
 "a"=>"3.0000000000000000", "b"=>"5.0000000000000000", "c"=>"6.0000000000000000"
(1 row)
```




