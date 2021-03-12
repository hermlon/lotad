#ifndef SESSION_CACHE_H
#define SESSION_CACHE_H

#include <stddef.h>

struct cache {
  char** data;
  size_t blocks;
  size_t blocksize;
};

struct cache* create_cache(size_t blocks, size_t blocksize);

void free_cache(struct cache* cache);

char* new_session(struct cache* cache);

void print_cache(struct cache* cache);
#endif
