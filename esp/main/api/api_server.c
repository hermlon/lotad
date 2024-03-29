#include "api_server.h"

#include <esp_http_server.h>
#include <esp_event.h>
#include <esp_log.h>

#include "api_controller.h"
#include "session_cache.h"

static const char *TAG = "api server";

#define SESSION_BLOCKS 10
#define SESSION_BLOCKSIZE 2

static httpd_handle_t api_server = NULL;

static void stop_webserver(httpd_handle_t server) {
	httpd_stop(server);
}

static void free_api_server_context(void* ctx) {
	ESP_LOGI(TAG, "Freeing api server ctx");
	free_cache(((api_server_context_t*) ctx)->session_cache);
	free(ctx);
}

static httpd_handle_t start_webserver() {
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.lru_purge_enable = true;

	/* api server context */
	api_server_context_t* api_context = calloc(1, sizeof(api_server_context_t));
	api_context->session_cache = create_cache(SESSION_BLOCKS, SESSION_BLOCKSIZE);
	config.global_user_ctx = api_context;
	config.global_user_ctx_free_fn = &free_api_server_context;

	// Start the httpd server
	ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
	if (httpd_start(&server, &config) == ESP_OK) {
			ESP_LOGI(TAG, "Registering URI handlers");

			// Set URI handlers
			api_controller_init(&server);
			return server;
	}

	ESP_LOGI(TAG, "Error starting server!");
	return NULL;
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	if(api_server) {
		ESP_LOGI(TAG, "Stopping webserver");
		stop_webserver(api_server);
		api_server = NULL;
	}
}

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	if(api_server == NULL) {
		ESP_LOGI(TAG, "Starting webserver");
		api_server = start_webserver();
	}
}

void start_api_server() {
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, NULL, NULL));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, NULL, NULL));

	api_server = start_webserver();
}
