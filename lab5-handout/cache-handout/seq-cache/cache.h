#include <stdio.h> 
#include "isa.h"
#ifndef CACHE_H
#define CACHE_H

typedef unsigned long long int mem_addr_t;

void initCache(int s_in, int b_in, int E_in);
void freeCache();
void accessData(mem_addr_t addr);

bool_t get_byte_cache(mem_t m, word_t pos, byte_t *dest);
bool_t get_word_cache(mem_t m, word_t pos, word_t *dest);
bool_t set_byte_cache(mem_t m, word_t pos, byte_t val);
bool_t set_word_cache(mem_t m, word_t pos, word_t val);

#endif /* CACHELAB_H */