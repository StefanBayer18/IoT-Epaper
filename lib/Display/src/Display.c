#ifndef DISPLAYESP
#define DISPLAYESP

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

struct Display {
    int width;
    int height;
    int reset_pin;
    int dc_pin;
    int cs_pin;
    int busy_pin;
};

void display_wait_idle(struct Display cfg){
    while (gpio_get_level(cfg.busy_pin)){
        vTaskDelay(3);
    }
    return;
}

#endif