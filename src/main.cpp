#include <DisplayDriver.h>
#include <GraphData.h>
#include <ImageDriver.h>

#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "IMAGEDRIVER"

void drawLayoutLines(ImageDriver& driver) {
    // Horizontal Lines
    driver.drawFilledRect({198, 0}, {4, driver.height()});
    driver.drawFilledRect({398, 0}, {4, driver.height() - 198});
    driver.drawFilledRect({598, 0}, {4, driver.height() - 198});

    driver.drawFilledRect({198, driver.height() - 202},
                          {driver.width() - 198, 4});
}

void drawBorderLines(ImageDriver& driver) {
    driver.drawFilledRect({0, 0}, {2, driver.height()});
    driver.drawFilledRect({driver.width() - 2, 0}, {2, driver.height()});

    driver.drawFilledRect({0, 0}, {driver.width(), 2});
    driver.drawFilledRect({0, driver.height() - 2}, {driver.width(), 2});
}

extern "C" void app_main() {
    printf("Hello World\n");
    DisplayDriver display{DOUT, SCLK, CS, DC, RST, BUSY};
    printf("Init Display\n");
    ImageDriver img{IMGWIDTH, IMGHEIGTH};
    printf("Init Image\n");
    const GraphData graph{GRAPHWIDTH};
    (void)graph;
    printf("Init Graph\n");
    // img.addPoint(12, 0);
    // img.addLine(0,0,800,480);
    // img.addFilledRect(11, 0, 200, 400);
    drawLayoutLines(img);
    drawBorderLines(img);
    img.drawText({32, 32}, "10°");
    printf("Added Rect\n");
    display.show(img);

    vTaskDelay(pdMS_TO_TICKS(100));

    display.clear(img.size());

    display.sleep();
    // img.debug();
}
