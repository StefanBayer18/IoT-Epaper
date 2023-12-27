#include <ImageDriver.h>
#include "driver/gpio.h"
#include <driver/spi_master.h>

#ifndef DISPLAYDRIVER

#define DISPLAYDRIVER
class DisplayDriver{
    public: 
    DisplayDriver(gpio_num_t rpin, gpio_num_t dc, gpio_num_t cs, gpio_num_t busy);
    void show(ImageDriver _img);
    void sleep();
    void waitIdle();
    void sendCommand(char cmd);
    void reset();
    void sendData(char data);

    private:
    ImageDriver img;
    int width;
    int height;
    gpio_num_t reset_pin;
    gpio_num_t dc_pin;
    gpio_num_t cs_pin;
    gpio_num_t busy_pin;
    spi_device_handle_t spi;
    void sendSPI(char data);
    void initSPI();
};

#endif