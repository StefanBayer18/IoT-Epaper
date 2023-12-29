#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DisplayDriver.h"
#include "driver/gpio.h"
#include <driver/spi_master.h>

DisplayDriver::DisplayDriver(gpio_num_t DIN, gpio_num_t SCLK, gpio_num_t CS, gpio_num_t DC, gpio_num_t RST, gpio_num_t BUSY)
{
    dout_pin = DIN;
    sclk = SCLK;
    cs_pin = CS;
    dc_pin = DC;
    reset_pin = RST;
    busy_pin = BUSY;

    printf("Building DisplayDriver\n");

    // Configure Pins
    ESP_ERROR_CHECK(gpio_set_direction(RST, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DC, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(CS, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(BUSY, GPIO_MODE_INPUT));

    ESP_ERROR_CHECK(gpio_set_level(cs_pin, 1));

    initSPI();

    initDisplay();
}

unsigned char Voltage_Frame_7IN5_V2[]={
	0x6, 0x3F, 0x3F, 0x11, 0x24, 0x7, 0x17,
};

void DisplayDriver::initDisplay(){
    // BWRmode & LUT from register (Page: 20)

    reset();

    // Booster Soft Start
    sendCommand(0x06);

    sendData(0x27);
	sendData(0x27);
	sendData(0x2F);
	sendData(0x17);

    // Power Setting (PWR) (R01h)
    sendCommand(0x01);
    sendData(0x17);
    sendData(0x23);
    sendData(0x3F);
    sendData(0x3F);
    sendData(0x11);

    sendCommand(0x04); // Turn device on
    //vTaskDelay(pdMS_TO_TICKS(100));
    waitIdle();
    printf("Done Waiting succesfully\n");
}

/**
 *  @brief: Displays given Image on the Display
 */
void DisplayDriver::show(ImageDriver img)
{
    sendCommand(0x13); // Data Start Transmission 2 (DTM2) (R13h)
    for (int x = 0; x < img.act_width * img.height; x++)
    {
        sendData(img.img[x]);
    }
    sendCommand(0x12); // Display Refresh (DRF) (R12h)
    vTaskDelay(pdMS_TO_TICKS(100));
    waitIdle();
}

void DisplayDriver::clear(int pixel){
    sendCommand(0x13); // Data Start Transmission 2 (DTM2) (R13h)
    for (int x = 0; x < pixel; x++)
    {
        sendData(0);
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
    ESP_ERROR_CHECK(gpio_set_level(dc_pin, 1));
    sendSPI(data);
}

/**
 *  @brief: Sends an Command to the Display
 */
void DisplayDriver::sendCommand(char cmd)
{
    ESP_ERROR_CHECK(gpio_set_level(dc_pin, 0));
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
        printf(".");
        sendCommand(0x71); // Get Status (FLG) (R71h)
        idle = gpio_get_level(busy_pin);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    printf("\n");
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
        .mosi_io_num = dout_pin,
        .miso_io_num = GPIO_NUM_12, // not used
        .sclk_io_num = sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0
    };

    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 0));

    spi_device_interface_config_t dev_config = {
        .command_bits = 0,
        .address_bits = 0,
        .mode = 0,                   // SPI mode (0, 1, 2, or 3)
        .clock_speed_hz = 1000000,  // Clock frequency
        .spics_io_num = cs_pin,  // Chip select pin
        .flags = 0,
        .queue_size = 1,
    };

    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev_config, &spi));
}

// TODO
void DisplayDriver::sendSPI(char data)
{
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &data
    };
    ESP_ERROR_CHECK(gpio_set_level(cs_pin, 0));

    //ESP_ERROR_CHECK(spi_device_transmit(spi, &t));
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));
    //ESP_ERROR_CHECK(spi_device_queue_trans(spi, &t, 100));
    
    ESP_ERROR_CHECK(gpio_set_level(cs_pin, 1));
}