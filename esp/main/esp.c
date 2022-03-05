#include <stdio.h>
#include "lwip/err.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_sntp.h"

#include "wifi_station.h"
#include "watering/watering.h"
#include "api/api_server.h"
#include "watering/water_control.h"
#include "github_ota.h"
#include "version.h"

/* The examples use WiFi configuration that you can set via project configuration menu
	 If you'd rather not, just change the below entries to strings with
	 the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY
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
	water_ctl_init();

	if(wifi_init_sta(EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_MAXIMUM_RETRY) == ESP_OK) {
		start_api_server();

        github_ota_check();

        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, EXAMPLE_SNTP_TIME_SERVER);
        sntp_init();
	}
	else {
		// start AP and allow for wifi scan
	}
}
