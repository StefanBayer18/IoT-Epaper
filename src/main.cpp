#include <DisplayDriver.h>
#include <ImageDriver.h>
#include <TestClass.h>
#include "esp_log.h"
#include "driver/gpio.h"

#define TAG "IMAGEDRIVER"

extern "C"
{
    void app_main();
}

void app_main()
{
    printf("Hello World\n");
    printf("Init Display\n");
    //ImageDriver img = ImageDriver();
    printf("Init Image\n");
    // img.addPoint(12, 0);
    // img.addLine(0,0,800,480);
    printf("Added Line\n");
    //DisplayDriver display = DisplayDriver(GPIO_NUM_17, GPIO_NUM_16, GPIO_NUM_2, GPIO_NUM_21);
    TestClass test = TestClass();
    test.prt();
    // display.show(img);
    // display.sleep();
    // img.debug();
    // display.show();
}
