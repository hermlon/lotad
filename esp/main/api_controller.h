#ifndef API_CONTROLLER_H
#define API_CONTROLLER_H

#include <esp_http_server.h>

void api_controller_register_uri_handlers(httpd_handle_t* server);

#endif
