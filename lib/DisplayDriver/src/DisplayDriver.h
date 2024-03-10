#ifndef DISPLAYDRIVER
#define DISPLAYDRIVER

#include <ImageDriver.h>
#include <driver/spi_master.h>

#include "driver/gpio.h"

// Code based on expample given by Waveshare slightly changed by Stefan
// https://www.waveshare.com/wiki/E-Paper_ESP32_Driver_Board
// https://files.waveshare.com/upload/6/60/7.5inch_e-Paper_V2_Specification.pdf

class DisplayDriver {
public:
    DisplayDriver(gpio_num_t DIN, gpio_num_t SCLK, gpio_num_t CS, gpio_num_t DC,
                  gpio_num_t RST, gpio_num_t BUSY);
    /**
     *  Displays given Image on the Display
     *  @param img Image to display from ImageDriver
     */
    void show(const ImageDriver& img) const;
    /**
     *  Puts Display into Deep Sleep mode
     */
    void sleep() const;
    /**
     *  Clears Display Image
     *  @param bytes Size of Display in Bytes
     */
    void clear(size_t pixel) const;
    /**
     *  Wait for Display to finish Interaction
     */
    bool waitIdle() const;
    /**
     *  Sends an Command to the Display
     * @param cmd Command to be send
     */
    void sendCommand(char cmd) const;
    /**
     *  Sends Data to the Display
     * @param data Data to be send
     */
    void sendData(uint8_t data) const;
    /**
     *  Resets Display Configuration
     */
    void reset() const;

private:
    gpio_num_t dout_pin;
    gpio_num_t sclk;
    gpio_num_t cs_pin;
    gpio_num_t dc_pin;
    gpio_num_t reset_pin;
    gpio_num_t busy_pin;
    spi_device_handle_t spi{};
    /**
     *  Sends Data to Display via SPI
     */
    void sendSPI(uint8_t data) const;

    /**
     *  Initialize SPI connection
     */
    void initSPI();

    /**
     * @brief Initializes the Display using given Values from example
     */
    void initDisplay() const;

    /**
     * @brief edits look up table with data based on example
     */
    void editLUT(uint8_t* lut_vcom, uint8_t* lut_ww, uint8_t* lut_bw,
                 uint8_t* lut_wb, uint8_t* lut_bb) const;
};

#endif
