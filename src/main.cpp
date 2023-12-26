#include <DisplayDriver.h>
#include <ImageDriver.h>
#include "esp_log.h"
#include "driver/gpio.h"

#define TAG "IMAGEDRIVER"

extern "C"
{
    void app_main()
    {
        ESP_LOGI(TAG, "TEST");
        DisplayDriver display = DisplayDriver(GPIO_NUM_17, GPIO_NUM_16, GPIO_NUM_2, GPIO_NUM_21);
        ImageDriver img = ImageDriver();
        img.addPoint(12, 0);
        display.sendData('a');
        display.show(img);
        img.debug();
        //display.show();
    }
}