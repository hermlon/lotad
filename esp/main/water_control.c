#include "water_control.h"

#include "esp_log.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "watering_storage.h"

static const char* TAG = "water ctl";

/* volatile */
static time_t last_watering = -1;
static time_t stoptime = -1;
static int counter[MAX_TIMERS];
static uint16_t water_accu = 0;

/* persistent */
static bool enabled = false;
static uint16_t capacity = 200;
static uint16_t savezone = 100;
static uint16_t throughput = 10;
static uint16_t current_capacity = 0;
static watering_t watering_config[MAX_TIMERS];

static void watering_task(void* params) {
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t wait_frequency = pdMS_TO_TICKS(60*1000);
    const TickType_t active_frequency = pdMS_TO_TICKS(2000);
    while(true) {

        if(water_ctl_get_active()) {
            time_t now;
            time(&now);
            ESP_LOGI("water_task", "active %ld", now);
        }
        xTaskDelayUntil(&last_wake_time, active_frequency);
    }
}

static void start_watering_task() {
    TaskHandle_t watering_task_handle = NULL;
    xTaskCreate(&watering_task, "watering_task", 2*1024, NULL, 1, &watering_task_handle);
    configASSERT(watering_task_handle);
}

static void store_active() {
	nvs_handle_t my_handle;
	ESP_ERROR_CHECK(nvs_open(STORE_NAMESPACE, NVS_READWRITE, &my_handle));

	ESP_ERROR_CHECK(nvs_set_u8(my_handle, STORE_ENABLED, (uint8_t) enabled));

	ESP_ERROR_CHECK(nvs_commit(my_handle));
	nvs_close(my_handle);
}

static void store_config() {
	nvs_handle_t my_handle;
	ESP_ERROR_CHECK(nvs_open(STORE_NAMESPACE, NVS_READWRITE, &my_handle));

	ESP_ERROR_CHECK(nvs_set_u16(my_handle, STORE_CAPACITY, capacity));
	ESP_ERROR_CHECK(nvs_set_u16(my_handle, STORE_SAVEZONE, savezone));
	ESP_ERROR_CHECK(nvs_set_u16(my_handle, STORE_THROUGHPUT, throughput));

	ESP_ERROR_CHECK(nvs_commit(my_handle));
	nvs_close(my_handle);
}

void water_ctl_init() {
	/* read values from nvs, if not available leave defaults unchanged */
	nvs_handle_t my_handle;
	esp_err_t result;
    if((result = nvs_open(STORE_NAMESPACE, NVS_READONLY, &my_handle)) != ESP_ERR_NVS_NOT_FOUND) {
        ESP_ERROR_CHECK(result);
    }
    if(result == ESP_OK) {
        if((result = nvs_get_u8(my_handle, STORE_ENABLED, (uint8_t *)&enabled)) != ESP_ERR_NVS_NOT_FOUND) {
            ESP_ERROR_CHECK(result);
        }
        if((result = nvs_get_u16(my_handle, STORE_CAPACITY, &capacity)) != ESP_ERR_NVS_NOT_FOUND) {
            ESP_ERROR_CHECK(result);
        }
        if((result = nvs_get_u16(my_handle, STORE_SAVEZONE, &savezone)) != ESP_ERR_NVS_NOT_FOUND) {
            ESP_ERROR_CHECK(result);
        }
        if((result = nvs_get_u16(my_handle, STORE_THROUGHPUT, &throughput)) != ESP_ERR_NVS_NOT_FOUND) {
            ESP_ERROR_CHECK(result);
        }
    } else {
        ESP_LOGI("water_control", "No stored config loaded");
    }

    start_watering_task();
}

void water_ctl_set_active(bool active) {
	if(active != enabled) {
		enabled = active;
		store_active();
	}
}

bool water_ctl_get_active() {
	return enabled;
}

void water_ctl_set_config(struct water_config config) {
	capacity = config.capacity;
	savezone = config.savezone;
	throughput = config.throughput;
	store_config();
}

struct water_config water_ctl_get_config() {
	struct water_config conf = {
		.capacity = capacity,
		.savezone = savezone,
		.throughput = throughput
	};
	return conf;
}
