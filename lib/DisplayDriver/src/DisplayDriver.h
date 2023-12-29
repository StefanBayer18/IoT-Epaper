#include <ImageDriver.h>
#include "driver/gpio.h"
#include <driver/spi_master.h>

#ifndef DISPLAYDRIVER

#define DISPLAYDRIVER
class DisplayDriver{
    public: 
    DisplayDriver(gpio_num_t DIN, gpio_num_t SCLK, gpio_num_t CS, gpio_num_t DC, gpio_num_t RST, gpio_num_t BUSY);
    void show(ImageDriver _img);
    void sleep();
    void waitIdle();
    void sendCommand(char cmd);
    void reset();
    void sendData(char data);

    private:
    gpio_num_t dout_pin;
    gpio_num_t sclk;
    gpio_num_t cs_pin;
    gpio_num_t dc_pin;
    gpio_num_t reset_pin;
    gpio_num_t busy_pin;
    spi_device_handle_t spi;
    void sendSPI(char data);
    void initSPI();
};

#endif