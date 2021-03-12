#ifndef SESSION_CACHE_H
#define SESSION_CACHE_H

#include <stddef.h>
#include <stdbool.h>

#define SESSION_ID_LENGTH 12

struct cache {
  char** data;
  size_t blocks;
  size_t blocksize;
};

struct cache* create_cache(size_t blocks, size_t blocksize);

void free_cache(struct cache* cache);

char* new_session(struct cache* cache);

bool contains_session(struct cache* cache, char session_id[SESSION_ID_LENGTH]);

void print_cache(struct cache* cache);
#endif
