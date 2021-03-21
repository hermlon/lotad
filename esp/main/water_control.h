#ifndef WATER_CONTROL_H
#define WATER_CONTROL_H

#include <time.h>
#include <stdbool.h>

#define MAX_TIMERS 5

typedef struct watering {
  time_t time;
  time_t offset;
  int amount;
} watering_t;

struct water_config {
  uint16_t capacity;
  uint16_t savezone;
  uint16_t throughput;
};

void water_ctl_init();

void water_ctl_set_active(bool active);
bool water_ctl_get_active();

void water_ctl_set_config(struct water_config config);
struct water_config water_ctl_get_config();

#endif
