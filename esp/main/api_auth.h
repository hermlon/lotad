#ifndef API_AUTH_H
#define API_AUTH_H

#include <esp_http_server.h>

bool authenticated(httpd_req_t* req);

#endif
