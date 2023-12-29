#include <DisplayDriver.h>
#include <ImageDriver.h>
#include <TestClass.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "IMAGEDRIVER"

#define DOUT GPIO_NUM_13
#define SCLK GPIO_NUM_14
#define CS GPIO_NUM_15
#define DC GPIO_NUM_27
#define RST GPIO_NUM_26
#define BUSY GPIO_NUM_25

extern "C"
{
    void app_main();
}

void app_main()
{
    printf("Hello World\n");
    DisplayDriver display = DisplayDriver(DOUT, SCLK, CS, DC, RST, BUSY);
    printf("Init Display\n");
    ImageDriver img = ImageDriver();
    printf("Init Image\n");
    // img.addPoint(12, 0);
    img.addLine(0,0,800,480);
    printf("Added Line\n");
    TestClass test = TestClass();
    test.prt();
    display.show(img);

    vTaskDelay(pdMS_TO_TICKS(100));

    display.clear(img.act_width * img.height);

    display.sleep();
    // img.debug();
    //display.reset();
}
