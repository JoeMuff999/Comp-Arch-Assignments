/* 
 * cache.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss. (I examined the trace,
 *  the largest request I saw was for 8 bytes).
 *  2. Instruction loads (I) are ignored, since we are interested in evaluating
 *  trans.c in terms of its data cache performance.
 *  3. data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus an possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work. 
 */
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "cachelab.h"
#include "cache.h"

//#define DEBUG_ON 
#define ADDRESS_LENGTH 64

/* Type: Memory address */
typedef unsigned long long int mem_addr_t;

typedef unsigned char byte_t;
typedef long long int word_t;

/* Globals set by command line args */
int verbosity_cache = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */

/* Derived from command line args */
int S; /* number of sets */
int B; /* block size (bytes) */

/* Counters used to record cache statistics in printSummary().
   test-cache uses these numbers to verify correctness of the cache. */

//Increment when a miss occurs
int miss_count = 0;

//Increment when a hit occurs
int hit_count = 0;

//Increment when an eviction occurs
int eviction_count = 0;

/* 
 * A possible hierarchy for the cache. The helper functions defined below
 * are based on this cache structure.
 * lru is a counter used to implement LRU replacement policy.
 */
typedef struct cache_line {
    char valid;
    mem_addr_t tag;
    unsigned long long int lru;
    byte_t *data;
} cache_line_t;

typedef struct cache_set {
    cache_line_t *lines;
} cache_set_t;

typedef struct cache {
    cache_set_t *sets;
} cache_t;

cache_t cache;

/* TODO: add more globals, structs, macros if necessary */
int tag_bits;
// int block_size;
// int num_sets;
// int line_in_set;


/* 
 * Initialize the cache according to specified arguments
 * Called by cache-runner so do not modify the function signature
 * 
 * The code provided here shows you how to initialize a cache structure
 * defined above. It's not complete and feel free to modify/add code.
 */
void initCache(int s_in, int b_in, int E_in)
{
    /* see cache-runner for the meaning of each argument */
    s = s_in;
    b = b_in;
    E = E_in;
    S = (unsigned int) pow(2, s);
    B = (unsigned int) pow(2, b);
    tag_bits = ADDRESS_LENGTH - (s + b);

    int i, j;
    cache.sets = (cache_set_t*) calloc(S, sizeof(cache_set_t));
    for (i=0; i<S; i++){
        cache.sets[i].lines = (cache_line_t*) calloc(E, sizeof(cache_line_t));
        for (j=0; j<E; j++){
            cache.sets[i].lines[j].valid = 0;
            cache.sets[i].lines[j].tag = 0;
            cache.sets[i].lines[j].lru = 0;
            cache.sets[i].lines[j].data = calloc(B, sizeof(byte_t));
        }
    }

    /* TODO: add more code for initialization */

}


/* 
 * Free allocated memory. Feel free to modify it
 */
void freeCache()
{
    int i;
    for (i=0; i<S; i++){
        free(cache.sets[i].lines);     
    }
    free(cache.sets);
}


/* TODO:
 * Check whether there is a cache hit (optional helper function)
 * On hit, return the cache line holding the address
 * On miss, returns NULL
 */
static cache_line_t *check_hit(word_t addr)
{
    unsigned int dummy = (unsigned int) addr;
    unsigned int addr_set = ((unsigned int) dummy << tag_bits) >> (tag_bits + b);
    unsigned int addr_tag = (dummy >> (s + b));
    unsigned int addr_block_offset = (dummy << (tag_bits + s)) >> (tag_bits + s);
    
    
    cache_line_t * to_return = NULL;
    for(int i = 0; i < E; i++)
    {
        if(cache.sets[addr_set].lines[i].valid == 1)
        {
            if(cache.sets[addr_set].lines[i].tag == addr_tag)
            {
                printf("hit addr :: %x , addr_set :: %x , addr_tag :: %x \n", addr, addr_set, addr_tag);
                to_return = &cache.sets[addr_set].lines[i];
                to_return->lru = 0;
                hit_count++;
            }
            else
            {
                cache.sets[addr_set].lines[i].lru++;
            }
        }
    }    

    return to_return;
}


/* TODO:
 * Handle a cache miss (optional helper function)
 * Return the cache line selected to filled in by addr
 */
static cache_line_t *handle_miss(word_t addr)
{
    unsigned int dummy = (unsigned int) addr;
    unsigned int addr_set = ((unsigned int) dummy << tag_bits) >> (tag_bits + b);
    unsigned int addr_tag = ((unsigned int) dummy >> (s + b));
    cache_line_t * lru_line = NULL;
    int max_so_far = -1;
    miss_count++;
    for(int i = 0; i < E; i++)
    {
        if(cache.sets[addr_set].lines[i].valid != 1)
        {
            cache.sets[addr_set].lines[i].valid = 1;
            cache.sets[addr_set].lines[i].tag = addr_tag;
            printf("cached :: %x , addr_set :: %x , addr_tag :: %x \n", addr, addr_set, addr_tag);
            return &cache.sets[addr_set].lines[i];
        }
            
        else
        {
            int curr = cache.sets[addr_set].lines[i].lru;
            assert(curr != max_so_far);
            if(curr > max_so_far)
            {
                max_so_far = curr;
                lru_line = &cache.sets[addr_set].lines[i];
                
            }
        }
    }
    printf("eviction :: %x , addr_set :: %x , addr_tag :: %x evicted = %x \n", addr, addr_set, addr_tag, lru_line->tag);
    lru_line->lru = 0;
    eviction_count++;
    lru_line->tag = addr_tag;
    return lru_line;
}


/* TODO:
 * Get a byte from memory and write to dest
 * On hit, read from cache directly
 * On miss, call get_byte_val() and update cache
 * Return TRUE on success
 */
bool_t get_byte_cache(mem_t m, word_t pos, byte_t *dest)
{
    if (pos < 0 || pos >= m->len) {
        return FALSE;
    }
    cache_line_t * line;
    /* your implementation */
    unsigned int addr_block_offset = ((unsigned int) pos << (tag_bits + s)) >> (tag_bits + s);
    
    if((line = check_hit(pos)) == NULL)
    {
        line = handle_miss(pos);
        get_byte_val(m, pos, dest);
        for(int i = 0; i < B; i++)
        {   
            get_byte_val(m, pos + i - addr_block_offset, (line->data + i));
        }
        
        return TRUE;
    }
    
    *dest = *(line->data + addr_block_offset) ;
    return TRUE;
}


/* TODO:
 * Get 8 bytes from memory and write to dest
 * On hit, read from cache directly
 * On miss, call get_word_val() and update cache
 * Return TRUE on success
 */
bool_t get_word_cache(mem_t m, word_t pos, word_t *dest) {
    if (pos < 0 || pos + 8 > m->len) {
        return FALSE;
    }

    cache_line_t * line;
    unsigned long int addr_block_offset = ((unsigned long int) pos << (tag_bits + s)) >> (tag_bits + s);
    /* your implementation */
    if((line = check_hit(pos)) == NULL)
    {
        
        line = handle_miss(pos);
        get_word_val(m,pos,dest);
        for(int i = 0; i < B; i++)
        {
            get_byte_val(m, pos + i - addr_block_offset, (line->data + i));
        }
        return TRUE;
    }
    
    *dest = *((word_t*)(line->data + addr_block_offset));
    return TRUE;
}


/* TODO:
 * Set a byte in cache, write through to memory with set_byte_val()
 * On miss, call get_byte_val() and update cache
 * Return TRUE on success
 */
bool_t set_byte_cache(mem_t m, word_t pos, byte_t val)
{
    if (pos < 0 || pos >= m->len)
	return FALSE;

    /* your implementation */
    unsigned int addr_block_offset = ((unsigned int) pos << (tag_bits + s)) >> (tag_bits + s);
    //write through to memory
    set_byte_val(m, pos, val);

    cache_line_t * line;
    //miss
    if((line = check_hit(pos)) == NULL)
    {
        line = handle_miss(pos);        
    }
    //write to cache
    *(line->data + addr_block_offset) = val;
    
    return TRUE;
}


/* TODO:
 * Set 8 bytes in cache, write through to memory with set_word_val()
 * On miss, call get_byte_val() and update cache
 * Return TRUE on success
 */
bool_t set_word_cache(mem_t m, word_t pos, word_t val)
{
    if (pos < 0 || pos + 8 > m->len)
	return FALSE;

    /* your implementation */
    unsigned int addr_block_offset = ((unsigned int) pos << (tag_bits + s)) >> (tag_bits + s);
    //write through to memory
    set_word_val(m, pos, val);
    
    cache_line_t * line;
    //miss
    if((line = check_hit(pos)) == NULL)
    {
        line = handle_miss(pos);        
    }
    assert(line != NULL);
    //write to cache
    *((word_t*)(line->data + addr_block_offset)) = val;
    
    return TRUE;
}


/* 
 * Access data at memory address addr
 * If it is already in cache, increast hit_count
 * If it is not in cache, bring it in cache, increase miss count
 * Also increase eviction_count if a line is evicted
 * 
 * Called by cache-runner; no need to modify it if you implement
 * check_hit() and handle_miss()
 */
void accessData(mem_addr_t addr)
{
    if(check_hit(addr) == NULL)
        handle_miss(addr);
}
