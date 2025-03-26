#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * Node structure for the doubly linked list used in LRU.
 * Each node holds a key (message ID), a pointer to the message value,
 * and pointers to the previous and next nodes in the list.
 */
typedef struct LRUNode {
    int key;               /* message ID */
    Message* value;        /* pointer to Message struct */
    struct LRUNode* prev;  /* previous node in list */
    struct LRUNode* next;  /* next node in list */
} LRUNode;

/* Head and tail dummy nodes for LRU doubly linked list */
static LRUNode* head = NULL;
static LRUNode* tail = NULL;

/* Current number of entries in cache */
static int current_size = 0;

/* Hash table mapping keys to nodes for O(1) access */
#define HASH_SIZE 103  /* A small prime number for decent distribution */
static LRUNode* hash_table[HASH_SIZE];

/* Cache hit/miss statistics */
static CacheStats stats;

/**
 * Simple hash function for mapping keys to indices.
 */
static int hash(int key) {
    return key % HASH_SIZE;
}

/****************************************************
 * init_cache
 * 
 * Initializes the LRU cache:
 *   - Sets up dummy head/tail for doubly linked list
 *   - Clears hash table
 *   - Resets stats
 ****************************************************/
void init_cache() {
    head = (LRUNode*)malloc(sizeof(LRUNode));
    tail = (LRUNode*)malloc(sizeof(LRUNode));
    head->next = tail;
    tail->prev = head;
    current_size = 0;
    stats.hits = stats.misses = 0;
    memset(hash_table, 0, sizeof(hash_table));
}

/**
 * Removes a node from its current position in the doubly linked list.
 */
static void remove_node(LRUNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

/**
 * Adds a node just before the tail (most recently used position).
 */
static void add_node_to_tail(LRUNode* node) {
    LRUNode* prev_last = tail->prev;
    prev_last->next = node;
    node->prev = prev_last;
    node->next = tail;
    tail->prev = node;
}

/****************************************************
 * evict_if_needed
 * 
 * If the cache is at full capacity, evicts the least recently
 * used node (the one right after head), removes it from the list,
 * deletes it from the hash table, and frees memory.
 ****************************************************/
static void evict_if_needed() {
    if (current_size < CACHE_CAPACITY) return;

    LRUNode* lru = head->next;
    if (lru == tail) return;  // Sanity check

    remove_node(lru);

    // Remove from hash table
    int h = hash(lru->key);
    while (hash_table[h] && hash_table[h]->key != lru->key) {
        h = (h + 1) % HASH_SIZE;
    }
    hash_table[h] = NULL;

    // Free message and node
    if (lru->value) free(lru->value);
    free(lru);
    current_size--;
}

/****************************************************
 * get_msg_from_cache_or_disk
 * 
 * Retrieves a message from cache if available, updating its
 * position in the LRU list. If not cached, loads from disk,
 * inserts it into the cache, and returns it.
 ****************************************************/
Message* get_msg_from_cache_or_disk(int msg_id) {
    int h = hash(msg_id);
    while (hash_table[h] && hash_table[h]->key != msg_id) {
        h = (h + 1) % HASH_SIZE;
    }

    if (hash_table[h] && hash_table[h]->key == msg_id) {
        // Cache hit
        stats.hits++;
        LRUNode* node = hash_table[h];
        remove_node(node);      // Remove from current position
        add_node_to_tail(node); // Move to MRU position
        return node->value;
    }

    // Cache miss
    stats.misses++;
    Message* msg = retrieve_msg(msg_id);
    if (!msg) return NULL;

    put_msg(msg);  // Add to cache and store to disk
    return msg;
}

/****************************************************
 * put_msg
 * 
 * Stores a message into the cache. If the message is already
 * cached, it updates the content and moves it to the MRU position.
 * If it's a new message, it inserts it, evicting the LRU if needed.
 * Also calls store_msg() for write-through persistence.
 ****************************************************/
void put_msg(const Message* msg) {
    int h = hash(msg->id);
    while (hash_table[h] && hash_table[h]->key != msg->id) {
        h = (h + 1) % HASH_SIZE;
    }

    if (hash_table[h]) {
        // Update existing
        LRUNode* node = hash_table[h];
        memcpy(node->value, msg, sizeof(Message));
        remove_node(node);
        add_node_to_tail(node);
    } else {
        // Insert new
        evict_if_needed();

        Message* new_msg = (Message*)malloc(sizeof(Message));
        memcpy(new_msg, msg, sizeof(Message));

        LRUNode* new_node = (LRUNode*)malloc(sizeof(LRUNode));
        new_node->key = msg->id;
        new_node->value = new_msg;

        add_node_to_tail(new_node);

        // Insert into hash table
        while (hash_table[h]) h = (h + 1) % HASH_SIZE;
        hash_table[h] = new_node;
        current_size++;
    }

    store_msg(msg);  // write-through to disk
}

/****************************************************
 * test_cache
 * 
 * Simulates a series of random message accesses and
 * collects statistics on cache hits/misses.
 ****************************************************/
void test_cache(int total_accesses, int num_messages, CacheStats* out_stats) {
    stats.hits = stats.misses = 0;
    for (int i = 0; i < total_accesses; i++) {
        int id = (rand() % num_messages) + 1;
        get_msg_from_cache_or_disk(id);
    }
    if (out_stats) *out_stats = stats;
}

/****************************************************
 * print_cache_stats
 * 
 * Outputs the total cache hits, misses, and hit ratio
 * to standard output.
 ****************************************************/
void print_cache_stats(const CacheStats* stats) {
    int total = stats->hits + stats->misses;
    double ratio = total ? (100.0 * stats->hits / total) : 0.0;
    printf("Cache Hits: %d\n", stats->hits);
    printf("Cache Misses: %d\n", stats->misses);
    printf("Hit Ratio: %.2f%%\n", ratio);
}
