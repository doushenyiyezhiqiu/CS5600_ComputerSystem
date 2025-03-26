/****************************************************
 * cache.h
 * Header for the cache system, including definitions
 * and function prototypes for caching messages and
 * handling LRU page replacement using a linked list.
 ****************************************************/

 #ifndef CACHE_H
 #define CACHE_H
 
 #include "message.h"
 
 #define CACHE_CAPACITY 16
 #define HASH_SIZE 103  // Prime number for better hash distribution
 
 typedef struct CacheStats {
     int hits;
     int misses;
 } CacheStats;
 
 /**
  * Initializes the LRU cache.
  */
 void init_cache();
 
 /**
  * Looks up a message in the cache or retrieves it from disk.
  * Updates the cache to mark it as most recently used.
  * @param msg_id the message ID to retrieve
  * @return pointer to message (do not free externally), or NULL on error
  */
 Message* get_msg_from_cache_or_disk(int msg_id);
 
 /**
  * Inserts a message into the cache and writes it to disk.
  * Evicts the least recently used message if cache is full.
  */
 void put_msg(const Message* msg);
 
 /**
  * Tests cache performance with simulated message access patterns.
  * @param total_accesses number of random accesses
  * @param num_messages total distinct message IDs to test
  * @param out_stats pointer to stats struct to populate
  */
 void test_cache(int total_accesses, int num_messages, CacheStats* out_stats);
 
 /**
  * Prints hit/miss stats and hit ratio.
  */
 void print_cache_stats(const CacheStats* stats);
 
 #endif /* CACHE_H */
 