#include "github_ota.h"

#include "version.h"

#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"

#define TAG "github ota"

#define URL_BUF_SIZE 128

extern const char github_root_cert_start[] asm("_binary_github_cert_pem_start");
extern const char github_root_cert_end[] asm("_binary_github_cert_pem_end");

static esp_err_t http_event_handler(esp_http_client_event_t* evt) {
    static int output_len;

    char** buffer = (char**)evt->user_data;

    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if(*buffer == NULL) {
                *buffer = (char*) malloc(esp_http_client_get_content_length(evt->client));
                output_len = 0;
                if(*buffer == NULL) {
                    ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                    return ESP_FAIL;
                }
            }
            memcpy(*buffer + output_len, evt->data, evt->data_len);
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

static bool get_latest_release(char* download_url) {
    char* buffer = NULL;

    esp_http_client_config_t config = {
            .host = "api.github.com",
            .path = "/repos/hermlon/lotad/releases/latest",
            .event_handler = http_event_handler,
            .user_data = &buffer,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .cert_pem = github_root_cert_start
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    bool new_url = false;
    if (err == ESP_OK) {
        if(esp_http_client_get_status_code(client) == 200) {
            cJSON* data_parsed = cJSON_Parse(buffer);
            cJSON* version = cJSON_GetObjectItemCaseSensitive(data_parsed, "name");
            if(cJSON_IsString(version) && (version->valuestring != NULL)) {
                if(strcmp(version->valuestring, VERSION_NAME) != 0) {
                    ESP_LOGI(TAG, "Found different version: current: %s, new: %s", VERSION_NAME, version->valuestring);
                    cJSON* assets = cJSON_GetObjectItemCaseSensitive(data_parsed, "assets");
                    if(cJSON_IsArray(assets)) {
                        cJSON* asset = NULL;
                        cJSON_ArrayForEach(asset, assets) {
                            ESP_LOGD(TAG, "foreach");
                            cJSON* name = cJSON_GetObjectItemCaseSensitive(asset, "name");
                            if(cJSON_IsString(name) && (name->valuestring) != NULL) {
                                if(strcmp(name->valuestring, "esp.bin") == 0) {
                                    cJSON* url = cJSON_GetObjectItemCaseSensitive(asset, "browser_download_url");
                                    if(cJSON_IsString(url) && (url->valuestring != NULL)) {
                                        strlcpy(download_url, url->valuestring, URL_BUF_SIZE);
                                        ESP_LOGI(TAG, "Download url: %s", download_url);
                                        new_url = true;
                                        break;
                                    }
                                }
                            }
                        }
                        ESP_LOGD(TAG, "end foreach");
                    }
                } else {
                    ESP_LOGI(TAG, "No new version found: current: %s, new: %s", VERSION_NAME, version->valuestring);
                }
            }
            cJSON_Delete(data_parsed);
        } else {
            ESP_LOGE(TAG, "HTTP GET Status = %d, message: %s", esp_http_client_get_status_code(client), buffer);
        }
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(buffer);

    return new_url;
}

static void ota_task(void* params) {
    char url[URL_BUF_SIZE];
    if(get_latest_release(url)) {
        ESP_LOGI(TAG, "Download url: %s", url);
    }

    vTaskDelete(NULL);
}

void github_ota_check() {
    xTaskCreate(&ota_task, "ota_task", 8*1024, NULL, 5, NULL);
}