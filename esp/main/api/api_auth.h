#ifndef API_AUTH_H
#define API_AUTH_H

#include <esp_http_server.h>
#include "session_cache.h"

bool authenticated(struct cache* session_cache, httpd_req_t* req);

#endif
