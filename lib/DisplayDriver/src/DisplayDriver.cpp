#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DisplayDriver.h"
#include "driver/gpio.h"

DisplayDriver::DisplayDriver(gpio_num_t rpin, gpio_num_t dc, gpio_num_t cs, gpio_num_t busy){
    reset_pin = rpin;
    dc_pin = dc;
    cs_pin = cs;
    busy_pin = busy;

    //Configure Pins
    gpio_set_direction(rpin, GPIO_MODE_OUTPUT);
    gpio_set_direction(dc, GPIO_MODE_OUTPUT);
    gpio_set_direction(cs, GPIO_MODE_OUTPUT);
    gpio_set_direction(busy_pin, GPIO_MODE_INPUT);
}


/**
 *  @brief: Displays given Image on the Display
 */
void DisplayDriver::show(ImageDriver _img){
    sendCommand(0x13); //Data Start Transmission 2 (DTM2) (R13h)
    for(int x = 0; x < width*height/8; x++){
            sendData(~img.img[x]);
    }
    sendCommand(0x12); //Display Refresh (DRF) (R12h)
    vTaskDelay(pdMS_TO_TICKS(100));
    wait_idle();
}

/**
 *  @brief: Sends Data to the Display
 */
void DisplayDriver::sendData(char data){
    gpio_set_level(dc_pin, 1);
    sendSPI(data);
}

/**
 *  @brief: Sends an Command to the Display
 */
void DisplayDriver::sendCommand(char cmd){
    gpio_set_level(dc_pin, 0);
    sendSPI(cmd);
}

void DisplayDriver::reset(){

}

//TODO
void DisplayDriver::wait_idle(){

}

//TODO
void DisplayDriver::sleep(){

}

//TODO
void sendSPI(char data){

}


