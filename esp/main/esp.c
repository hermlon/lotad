#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "protocomm.h"
#include "esp_event.h"

#include "watering/watering.h"
#include "api/api_server.h"
#include "watering/water_control.h"
#include "github_ota.h"
#include "wifi_provision.h"
#include "version.h"

#define EXAMPLE_SNTP_TIME_SERVER   CONFIG_SNTP_TIME_SERVER

#define TAG "esp main"

void nvs_init() {
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ESP_ERROR_CHECK(nvs_flash_init());
	}
    ESP_ERROR_CHECK(ret);
}

void app_main(void) {
    ESP_LOGI(TAG, "Starting lotad version %s", VERSION_NAME);
	nvs_init();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_provision_start();

    start_api_server();

    github_ota_check();

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, EXAMPLE_SNTP_TIME_SERVER);
    sntp_init();

    water_ctl_init();
}