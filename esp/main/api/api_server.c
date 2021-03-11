#include "api_server.h"

#include <esp_http_server.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>

#include "api_controller.h"

static const char *TAG = "api server";

static httpd_handle_t server = NULL;

static void stop_webserver(httpd_handle_t server) {
  httpd_stop(server);
}

static httpd_handle_t start_webserver() {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.lru_purge_enable = true;

  // Start the httpd server
  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK) {
      ESP_LOGI(TAG, "Registering URI handlers");

      // Set URI handlers
      api_controller_register_uri_handlers(&server);
      return server;
  }

  ESP_LOGI(TAG, "Error starting server!");
  return NULL;
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  if(server) {
    ESP_LOGI(TAG, "Stopping webserver");
    stop_webserver(server);
    server = NULL;
  }
}

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  if(server == NULL) {
    ESP_LOGI(TAG, "Starting webserver");
    server = start_webserver();
  }
}

void start_api_server() {
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, NULL, NULL));

  server = start_webserver();
}
