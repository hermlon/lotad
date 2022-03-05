#include "github_ota.h"

#include "version.h"

#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"

#define MAX_HTTP_OUTPUT_BUFFER 2048
#define TAG "github ota"

extern const char github_root_cert_start[] asm("_binary_github_cert_pem_start");
extern const char github_root_cert_end[] asm("_binary_github_cert_pem_end");

static esp_err_t http_event_handler(esp_http_client_event_t* evt) {
    static int output_len;

    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            memcpy(evt->user_data + output_len, evt->data, evt->data_len);
            output_len += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            output_len = 0;
            break;
        default:
            break;
    }
    return ESP_OK;
}

static bool get_latest_release() {
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    esp_http_client_config_t config = {
            .url = "https://api.github.com/repos/hermlon/lotad/releases/latest",
            .event_handler = http_event_handler,
            .user_data = local_response_buffer,
            .cert_pem = github_root_cert_start
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));

    esp_http_client_cleanup(client);

    return false;
}

static void ota_task(void* params) {
    get_latest_release();

    vTaskDelete(NULL);
}

void github_ota_check() {
    xTaskCreate(&ota_task, "ota_task", 8*1024, NULL, 5, NULL);
}