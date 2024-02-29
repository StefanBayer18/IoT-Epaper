#ifndef DISPLAYDRIVER
#define DISPLAYDRIVER

#include <ImageDriver.h>
#include "driver/gpio.h"
#include <driver/spi_master.h>

class DisplayDriver {
public:
    DisplayDriver(gpio_num_t DIN, gpio_num_t SCLK, gpio_num_t CS, gpio_num_t DC,
                  gpio_num_t RST, gpio_num_t BUSY);
    void show(const ImageDriver& img) const;
    void sleep() const;
    void clear(size_t pixel) const;
    bool waitIdle() const;
    void sendCommand(char cmd) const;
    void sendData(uint8_t data) const;
    void reset() const;

private:
    gpio_num_t dout_pin;
    gpio_num_t sclk;
    gpio_num_t cs_pin;
    gpio_num_t dc_pin;
    gpio_num_t reset_pin;
    gpio_num_t busy_pin;
    spi_device_handle_t spi{};
    void sendSPI(uint8_t data) const;
    void initSPI();
    void initDisplay() const;
    void editLUT(uint8_t* lut_vcom,  uint8_t* lut_ww, uint8_t* lut_bw, uint8_t* lut_wb, uint8_t* lut_bb) const;
};

#endif
