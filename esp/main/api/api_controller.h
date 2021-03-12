#ifndef API_CONTROLLER_H
#define API_CONTROLLER_H

#include <esp_http_server.h>

void api_controller_init(httpd_handle_t* server);

void api_controller_deinit();

#endif
