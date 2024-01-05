#include <DisplayDriver.h>
#include <ImageDriver.h>
#include <GraphData.h>
#include "config.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "IMAGEDRIVER"

extern "C" {
void app_main();
}

void app_main() {
    printf("Hello World\n");
    DisplayDriver display{DOUT, SCLK, CS, DC, RST, BUSY};
    printf("Init Display\n");
    const ImageDriver img{IMGWIDTH, IMGHEIGTH};
    printf("Init Image\n");
    const GraphData graph{GRAPHWIDTH};
    (void)graph;
    printf("Init Graph\n");
    // img.addPoint(12, 0);
    // img.addLine(0,0,800,480);
    //img.addFilledRect(11, 0, 200, 400);
    img.addLayoutLines();
    img.addBorderLines();
    printf("Added Rect\n");
    display.show(img);

    vTaskDelay(pdMS_TO_TICKS(100));

    display.clear(img.imgSize);

    display.sleep();
    // img.debug();
}
