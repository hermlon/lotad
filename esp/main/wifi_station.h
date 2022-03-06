#ifndef LOTAD_WIFI_STATION_H
#define  LOTAD_WIFI_STATION_H

#include <stdint.h>
#include "lwip/err.h"

esp_err_t wifi_init_sta(char wifi_ssid[32], char wifi_pass[64], int retries);
#endif
