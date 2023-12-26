#include <ImageDriver.h>
#include "driver/gpio.h"

class DisplayDriver{
    public: 
    DisplayDriver(gpio_num_t rpin, gpio_num_t dc, gpio_num_t cs, gpio_num_t busy);
    void show(ImageDriver _img);
    void sleep();
    void wait_idle();
    void sendCommand(char cmd);
    void reset();
    void sendData(char data);

    private:
    void sendSPI(char data);
    ImageDriver img;
    int width;
    int height;
    gpio_num_t reset_pin;
    gpio_num_t dc_pin;
    gpio_num_t cs_pin;
    gpio_num_t busy_pin;
};