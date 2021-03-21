#ifndef WATER_CONTROL_H
#define WATER_CONTROL_H

#include <time.h>
#include <stdbool.h>

typedef struct watering {
  time_t time;
  time_t offset;
  int amount;
} watering_t;

void water_ctl_init();

void water_ctl_set_active(bool active);
bool water_ctl_get_active();

void water_ctl_store();

#endif
