#include "water_control.h"

#include "esp_timer.h"
#include "esp_log.h"
#include "nvs.h"

#include "watering_storage.h"

#define MAX_TIMERS 5

static const char* TAG = "water ctl";

/* volatile */
static time_t last_watering = -1;
static time_t stoptime = -1;
static int counter[MAX_TIMERS];
static float water_accu = 0;

/* persistent */
static bool enabled = false;
static float capacity = 200.0;
static float savezone = 100.0;
static float current_capacity = 0.0;
static float throughput = 10.0;
static watering_t watering_config[MAX_TIMERS];

static void start_watering_timer() {

}

static void s_store_active(nvs_handle_t* my_handle) {
  ESP_ERROR_CHECK(nvs_set_i8(*my_handle, STORE_ENABLED, (int8_t) enabled));
}

static void store_active() {
  nvs_handle_t my_handle;
  ESP_ERROR_CHECK(nvs_open(STORE_NAMESPACE, NVS_READWRITE, &my_handle));
  s_store_active(&my_handle);
  ESP_ERROR_CHECK(nvs_commit(my_handle));
  nvs_close(my_handle);
}

void water_ctl_init() {
  /* read values from nvs */
  nvs_handle_t my_handle;
  ESP_ERROR_CHECK(nvs_open(STORE_NAMESPACE, NVS_READONLY, &my_handle));
  esp_err_t result;
  if((result = nvs_get_i8(my_handle, STORE_ENABLED, (int8_t *)&enabled)) != ESP_ERR_NVS_NOT_FOUND) {
    ESP_ERROR_CHECK(result);
  }
  ESP_LOGI(TAG, "enabled: %d", enabled);

  if(enabled) {
    start_watering_timer();
  }
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

void water_ctl_store() {
  /* store values to nvs */
  nvs_handle_t my_handle;
  ESP_ERROR_CHECK(nvs_open(STORE_NAMESPACE, NVS_READWRITE, &my_handle));
  s_store_active(&my_handle);
  /*
  store_current_capacity();
  store_throughput();
  store_watering_config();*/
  ESP_ERROR_CHECK(nvs_commit(my_handle));
  nvs_close(my_handle);
}
