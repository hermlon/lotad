#include "watering.h"

#include "driver/gpio.h"

#define GPIO_OUTPUT_RELAIS  18
#define GPIO_OUTPUT_PIN_SEL (1ULL<<GPIO_OUTPUT_RELAIS)

void water_gpio_config() {
	gpio_config_t io_conf;
	// disable interrupt
	io_conf.intr_type = GPIO_INTR_DISABLE;
	// set as output mode
	io_conf.mode = GPIO_MODE_OUTPUT;
	// bit mask of the pins that you want to set
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	// disable pull-down mode
	io_conf.pull_down_en = 0;
	// disable pull-up mode;
	io_conf.pull_up_en = 0;
	// configure GPIO with the given settings
	gpio_config(&io_conf);
}

void water_on() {
	gpio_set_level(GPIO_OUTPUT_RELAIS, 0);
}

void water_off() {
	gpio_set_level(GPIO_OUTPUT_RELAIS, 1);
}
