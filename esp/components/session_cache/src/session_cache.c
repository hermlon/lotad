#include "session_cache.h"

#include <stdlib.h>
#include <string.h>
#include "esp_system.h"

#define SESSION_ID_LENGTH 12

const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* blocks have to be less than 64 */
struct cache* create_cache(size_t blocks, size_t blocksize) {
  char** data = malloc(blocks * blocksize * sizeof(char*));
  for(int i = 0; i < blocks * blocksize; i ++) {
    data[i] = calloc(SESSION_ID_LENGTH, sizeof(char));
  }
  struct cache* cache = malloc(sizeof(struct cache));
  cache->data = data;
  cache->blocks = blocks;
  cache->blocksize = blocksize;
  return cache;
}

void free_cache(struct cache* cache) {
  for(int i = 0; i < cache->blocks * cache->blocksize; i ++) {
    free(cache->data[i]);
  }
  free(cache->data);
  free(cache);
}

static void generate_random_session_id(char session_id[SESSION_ID_LENGTH]) {
  uint32_t random = esp_random();
  for(int i = 0; i < SESSION_ID_LENGTH - 1; i ++) {
    session_id[i] = b64chars[random%64];
    random /= 64;
    if(random == 0) {
      random = esp_random();
    }
  }
  session_id[SESSION_ID_LENGTH-1] = '\0';
}

char* new_session(struct cache* cache) {
  char session_id[SESSION_ID_LENGTH];
  generate_random_session_id(session_id);

  /* store inside cache */
  /* less blocks than 64 */
  int block = (session_id[0] % cache->blocks) * cache->blocksize;
  int blockline = 0;
  while(blockline < cache->blocksize - 1 && cache->data[block+blockline][0] != '\0') {
    blockline ++;
  }

  /* didn't find an empty line, use a random one */
  if(cache->data[block+blockline][0] != '\0') {
    blockline = esp_random() % cache->blocksize;
  }

  /* store new session id in cache */
  strlcpy(cache->data[block+blockline], session_id, SESSION_ID_LENGTH);
  return cache->data[block+blockline];
}

void print_cache(struct cache* cache) {
  for(int i = 0; i < cache->blocks * cache->blocksize; i ++) {
    printf("%d: %s\n",i, cache->data[i]);
  }
}
