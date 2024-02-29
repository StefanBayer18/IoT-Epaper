#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DisplayDriver.h"
#include "driver/gpio.h"
#include <driver/spi_master.h>
#include "esp_log.h"

#define TAG "DISPLAYDRIVER"

uint8_t LUT_VCOM_7IN5_V2[]={	
	0x0,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0xF,	0x1,	0xF,	0x1,	0x2,	
	0x0,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};						

uint8_t LUT_WW_7IN5_V2[]={	
	0x10,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x84,	0xF,	0x1,	0xF,	0x1,	0x2,	
	0x20,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};

uint8_t LUT_BW_7IN5_V2[]={	
	0x10,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x84,	0xF,	0x1,	0xF,	0x1,	0x2,	
	0x20,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};

uint8_t LUT_WB_7IN5_V2[]={	
	0x80,	0xF,	0xF,	0x0,	0x0,	0x3,	
	0x84,	0xF,	0x1,	0xF,	0x1,	0x4,	
	0x40,	0xF,	0xF,	0x0,	0x0,	0x3,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};

uint8_t LUT_BB_7IN5_V2[]={	
	0x80,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x84,	0xF,	0x1,	0xF,	0x1,	0x2,	
	0x40,	0xF,	0xF,	0x0,	0x0,	0x1,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	
};


DisplayDriver::DisplayDriver(const gpio_num_t DIN, const gpio_num_t SCLK,
                             const gpio_num_t CS, const gpio_num_t DC,
                             const gpio_num_t RST, const gpio_num_t BUSY) {
    dout_pin = DIN;
    sclk = SCLK;
    cs_pin = CS;
    dc_pin = DC;
    reset_pin = RST;
    busy_pin = BUSY;

    ESP_LOGI(TAG, "Building DisplayDriver\n");

    // Configure Pins
    ESP_ERROR_CHECK(gpio_set_direction(RST, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DC, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(CS, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(BUSY, GPIO_MODE_INPUT));

    initSPI();

    initDisplay();
}

void DisplayDriver::editLUT(uint8_t* lut_vcom,  uint8_t* lut_ww, uint8_t* lut_bw, uint8_t* lut_wb, uint8_t* lut_bb) const{
    uint16_t count = 0;
    sendCommand(0x20); //VCOM	
	for(count=0; count<60; count++)
		sendData(lut_vcom[count]);

	sendCommand(0x21); //LUTBW
	for(count=0; count<60; count++)
		sendData(lut_ww[count]);

	sendCommand(0x22); //LUTBW
	for(count=0; count<60; count++)
		sendData(lut_bw[count]);

	sendCommand(0x23); //LUTWB
	for(count=0; count<60; count++)
		sendData(lut_wb[count]);

	sendCommand(0x24); //LUTBB
	for(count=0; count<60; count++)
		sendData(lut_bb[count]);
}

void DisplayDriver::initDisplay() const {
    // BWRmode & LUT from register (Page: 20)

    reset();

    // Power Setting (PWR) (R01h)
    sendCommand(0x01);
    sendData(0x17);
    sendData(0x17);
    sendData(0x3F);
    sendData(0x3F);
    sendData(0x11);

    sendCommand(0x82);  // VCOM DC Setting
	sendData(0x24);  // VCOM

    // Booster Soft Start
    sendCommand(0x06);
    sendData(0x27);
    sendData(0x27);
    sendData(0x2F);
    sendData(0x17);

    do{
    sendCommand(0x04); // Turn device on
    vTaskDelay(pdMS_TO_TICKS(100));
    }
    while(!waitIdle());

    //sendCommand(0x30);
    //sendData(0x6);

    
    //vTaskDelay(pdMS_TO_TICKS(100));

    sendCommand(0X00);			//PANNEL SETTING BRIGGS
    sendData(0x3F);

    sendCommand(0x61);        	//tres
    sendData(0x03);		//source 800
    sendData(0x20);
    sendData(0x01);		//gate 480
    sendData(0xE0);

    sendCommand(0X15);
    sendData(0x00);

    sendCommand(0X50);			//VCOM AND DATA INTERVAL SETTING
    sendData(0x10);
    sendData(0x00);

    sendCommand(0X60);			//TCON SETTING
    sendData(0x22);

    sendCommand(0x65);  // Resolution setting
    sendData(0x00);
    sendData(0x00);//800*480
    sendData(0x00);
    sendData(0x00);

    editLUT(LUT_VCOM_7IN5_V2, LUT_WW_7IN5_V2, LUT_BW_7IN5_V2, LUT_WB_7IN5_V2, LUT_BB_7IN5_V2);

    ESP_LOGI(TAG, "Done Waiting succesfully\n");
}

/**
 *  Displays given Image on the Display
 *  @param img Image to display from ImageDriver
 */
void DisplayDriver::show(const ImageDriver& img) const {
    sendCommand(0x10); // Data Start Transmission 1 (DTM1) (R13h)
    for (int x = 0; x < img.size(); x++) {
        sendData(img[x]);
    }
    sendCommand(0x13); // Data Start Transmission 2 (DTM2) (R13h)
    for (int x = 0; x < img.size(); x++) {
        sendData(img[x]);
    }
    sendCommand(0x12); // Display Refresh (DRF) (R12h)
    vTaskDelay(pdMS_TO_TICKS(100));
    waitIdle();
}

/**
 *  Clears Display Image
 *  @param bytes Amount of Bytes of Display
 */
void DisplayDriver::clear(size_t bytes) const {
    sendCommand(0x13); // Data Start Transmission 2 (DTM2) (R13h)
    for (int x = 0; x < bytes; x++) {
        sendData(0);
    }
    sendCommand(0x12); // Display Refresh (DRF) (R12h)
    vTaskDelay(pdMS_TO_TICKS(100));
    waitIdle();
}

/**
 *  Sends Data to the Display
 */
void DisplayDriver::sendData(uint8_t data) const {
    ESP_ERROR_CHECK(gpio_set_level(dc_pin, 1));
    sendSPI(data);
}

/**
 *  Sends an Command to the Display
 */
void DisplayDriver::sendCommand(char cmd) const {
    ESP_ERROR_CHECK(gpio_set_level(dc_pin, 0));
    sendSPI(cmd);
}

/**
 *  Resets Display Configuration
 */
void DisplayDriver::reset() const {
    gpio_set_level(reset_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(reset_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(4));
    gpio_set_level(reset_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(200));
}

/**
 *  Wait for Display to finish Interaction
 */
bool DisplayDriver::waitIdle() const {
    vTaskDelay(pdMS_TO_TICKS(20));
    int idle = 0;
    int x = 0;
    while (idle == 0) {
        printf(".");
        sendCommand(0x71); // Get Status (FLG) (R71h)
        idle = gpio_get_level(busy_pin);
        vTaskDelay(pdMS_TO_TICKS(50));
        x++;
        if(x > 1000){
            printf("Couldnt start Display-------------------\n");
            return false;
        }
    }
    printf("\n");
    return true;
}

/**
 *  Puts Display into Deep Sleep mode
 */
void DisplayDriver::sleep() const {
    sendCommand(0X02); // Power OFF (POF) (R02h)
    waitIdle();
    sendCommand(0X07); // Deep Sleep (DSLP) (R07h)
    sendData(0xA5);    // check code = 0xA5
}

/**
 *  Initialize SPI connection
 */
void DisplayDriver::initSPI() {
    spi_bus_config_t buscfg{};
    buscfg.mosi_io_num = dout_pin;
    buscfg.miso_io_num = -1; // not used
    buscfg.sclk_io_num = sclk;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = 0;

    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 0));

    spi_device_interface_config_t dev_config{};
    dev_config.mode = 0;                 // SPI mode (0, 1, 2, or 3)
    dev_config.clock_speed_hz = 2000000; // Clock frequency
    dev_config.spics_io_num = cs_pin;    // Chip select pin
    dev_config.queue_size = 1;

    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev_config, &spi));
}

/**
 *  Sends Data to Display via SPI
 */
void DisplayDriver::sendSPI(const uint8_t data) const {
    spi_transaction_t t{};
    t.length = 8;
    t.tx_buffer = &data;

    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));
}
