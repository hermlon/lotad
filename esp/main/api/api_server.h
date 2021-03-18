#ifndef API_SERVER_H
#define API_SERVER_H

#define SCRATCH_BUFSIZE 10240

typedef struct api_server_context {
  struct cache* session_cache;
  char scratch[SCRATCH_BUFSIZE];
  unsigned char password_hash[32];
} api_server_context_t;

void start_api_server();

#endif
