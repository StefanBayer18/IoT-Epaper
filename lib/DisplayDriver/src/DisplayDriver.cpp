#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DisplayDriver.h"
#include "driver/gpio.h"
#include <driver/spi_master.h>

DisplayDriver::DisplayDriver(gpio_num_t rpin, gpio_num_t dc, gpio_num_t cs, gpio_num_t busy)
{
    reset_pin = rpin;
    dc_pin = dc;
    cs_pin = cs;
    busy_pin = busy;

    // Configure Pins
    gpio_set_direction(rpin, GPIO_MODE_OUTPUT);
    gpio_set_direction(dc, GPIO_MODE_OUTPUT);
    gpio_set_direction(cs, GPIO_MODE_OUTPUT);
    gpio_set_direction(busy_pin, GPIO_MODE_INPUT);

    sendCommand(0x04); // Turn device on
    vTaskDelay(pdMS_TO_TICKS(100));
    waitIdle();

    initSPI();
}

/**
 *  @brief: Displays given Image on the Display
 */
void DisplayDriver::show(ImageDriver _img)
{
    sendCommand(0x13); // Data Start Transmission 2 (DTM2) (R13h)
    for (int x = 0; x < width * height / 8; x++)
    {
        sendData(~img.img[x]);
    }
    sendCommand(0x12); // Display Refresh (DRF) (R12h)
    vTaskDelay(pdMS_TO_TICKS(100));
    waitIdle();
}

/**
 *  @brief: Sends Data to the Display
 */
void DisplayDriver::sendData(char data)
{
    gpio_set_level(dc_pin, 1);
    sendSPI(data);
}

/**
 *  @brief: Sends an Command to the Display
 */
void DisplayDriver::sendCommand(char cmd)
{
    gpio_set_level(dc_pin, 0);
    sendSPI(cmd);
}

void DisplayDriver::reset()
{
    gpio_set_level(reset_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(reset_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(4));
    gpio_set_level(reset_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(20));
}

void DisplayDriver::waitIdle()
{
    vTaskDelay(pdMS_TO_TICKS(20));
    char idle = 0;
    while (idle == 0)
    {
        sendCommand(0x71); // Get Status (FLG) (R71h)
        idle = gpio_get_level(busy_pin);
    }
}

void DisplayDriver::sleep()
{
    sendCommand(0X02); // Power OFF (POF) (R02h)
    waitIdle();
    sendCommand(0X07); // Deep Sleep (DSLP) (R07h)
    sendData(0xA5);    // check code = 0xA5
}

void DisplayDriver::initSPI()
{
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_NUM_5,
        .miso_io_num = -1, // not used
        .sclk_io_num = GPIO_NUM_18,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0
    };

    esp_err_t ret = spi_bus_initialize(HSPI_HOST, &buscfg, 0);
    assert(ret == ESP_OK);

    spi_device_interface_config_t dev_config = {
        .command_bits = 0,
        .address_bits = 0,
        .mode = 0,                   // SPI mode (0, 1, 2, or 3)
        .clock_speed_hz = 2000000,  // Clock frequency
        .spics_io_num = cs_pin,  // Chip select pin
        .flags = 0,
        .queue_size = 1,
    };

    ret = spi_bus_add_device(HSPI_HOST, &dev_config, &spi);
    assert(ret == ESP_OK);
}

// TODO
void DisplayDriver::sendSPI(char data)
{
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data
    };

    esp_err_t ret = spi_device_transmit(spi, &t);
    assert(ret == ESP_OK);
}