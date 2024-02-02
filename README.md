<overall Flow>
-1. We have to initialize contents of a main memory and a cache memory by invoking
init_memory_content() and init_cache_content().
-2. Open access_input.txt and read each line by line.
-3. Get the data by invoking retrieve_data. First, we check that the data is in cache by
check_cache_data_hit(). If there is a data in cache, we can use the cache’s data directly. But there
is not a data in cache, we have to find the data in memory by access_memory() and save the data
in cache. In this case, we use find_entry_index_in_set() to find appropriate location.
-4.print access_output.txt and hit ratio and the band width.
  
<cache.c – check_cache_data_hit()>
-1. We have to check all entries in set by using for function and repeat function.
-2. Check all entries to find matching tag value. If you find correct tag value, Run the type_check
function to return the hit data in the cache set and modify cache’s timestamp to
global_timestamp. If you can’t find matching tag value, we define return value -1 to express ‘miss’
state.

<cache.c-find_entry_index_in_set()>
-1. First, we define entry_index to -1. If there is no entry index, we have to know that. So we use ‘-
1’which is not use for cache’s index.
-2. If we use directed-mapped, the cache set size is 1, so we must use 0 index.
-3. If else, we repeat the function to find entry index. The function is that valid value is zero, we
modify the valid value.
-4. If there is no entry index, we have to find the least recently used entry by checking
‘timestamp’ and modify entry_index value

<cache.c-acess_memory()>
-1. We have to calculate word address, block offset, cache’s index, cache’s tag, cache’s entry index.
-2. Fetch one block in memory and copy to cache.
-3. Using word index, receive the word in memory two time. Because the cache read 2 word in
onetime.
-4. Store the cache’s data to byte unit.
-5. Modify tag, valid and timestamp

<cache.c-type_check()>
-1.define the variable to receive data. Because we have to use reverse order data. So we use new
variable to store data to reverse order.
-2. we have to print n byte data. So, add n num_bytes and receive the data in cache to char array
per n time, and combine the data.

<main.c-retrieve_data()>
-1. First, Make the variable which store data. What you need to be careful about is that we return
the int type data.
-2. If the data is in cache, we can use the data right away.(check the ‘hit’ situation.) But if the data
is not in cache, we access to memory, and access to cache. (check the ‘miss’ situation.)

<main.c – main()>
-1. Read address and type line by line and store access_addr and access_type.
-2. For using access_addr and access_type, play retrieve_data() function. and we approach to
cache, so we add 1 to global_timestamp.
-3. Calculate hit ratio by using num_cache_hits and num_cache_misses.
-4. Calculate bandwidth by using num_bytes and num_access_cycles.
