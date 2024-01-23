#include <DisplayDriver.h>
#include <GraphData.h>
#include <ImageDriver.h>
#include "sunnyImg.h"

#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "IMAGEDRIVER"

void drawLayoutLines(ImageDriver& driver) {
    // Vertikal Lines
    driver.drawFilledRect({198, 0}, {4, driver.height()});
    driver.drawFilledRect({398, 0}, {4, driver.height() - 198});
    driver.drawFilledRect({598, 0}, {4, driver.height() - 198});

    driver.drawFilledRect({198, driver.height() - 202},
                          {driver.width() - 198, 4});
}

void drawBorderLines(ImageDriver& driver) {
    //Horizontal
    driver.drawFilledRect({0, 0}, {4, driver.height()});
    driver.drawFilledRect({driver.width() - 4, 0}, {4, driver.height()});

    //Vertikal
    driver.drawFilledRect({0, 0}, {driver.width(), 4});
    driver.drawFilledRect({0, driver.height() - 4}, {driver.width(), 4});
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
    //img.drawFilledRect({80, 0}, {7, img.height()});
    //img.drawFilledRect({0, 0}, {4, img.height()});
    //img.drawFilledRect({0,0},{7,10});
    img.drawImage({236, 30}, {sunnyImg, 128});
    img.drawImage({700, 400}, {sunnyImg, 128});
    printf("Image drawn\n");
    img.drawText({32, 32}, "10");
    //printf("Added Rect\n");
    display.show(img);

    vTaskDelay(pdMS_TO_TICKS(5000));

    display.clear(img.size());

    display.sleep();
    // img.debug();
}
