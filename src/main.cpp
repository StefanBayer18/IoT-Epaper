#include <DisplayDriver.h>
#include <ImageDriver.h>
#include "esp_log.h"
#define TAG "IMAGEDRIVER"

extern "C"
{
    void app_main()
    {
        ESP_LOGI(TAG, "TEST");
        DisplayDriver display = DisplayDriver(1,1,1,1);
        ImageDriver img = ImageDriver();
        img.addPoint(12, 0);
        display.sendData('a');
        display.show(img);
        img.debug();
        //display.show();
    }
}