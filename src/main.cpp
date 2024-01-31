#include <DisplayDriver.h>
#include <GraphData.h>
#include <ImageDriver.h>
#include <WifiDriver.h>
#include <APIData.h>

#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sunnyImg.h"

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
    // Horizontal
    driver.drawFilledRect({0, 0}, {4, driver.height()});
    driver.drawFilledRect({driver.width() - 4, 0}, {4, driver.height()});

    // Vertikal
    driver.drawFilledRect({0, 0}, {driver.width(), 4});
    driver.drawFilledRect({0, driver.height() - 4}, {driver.width(), 4});
}

void drawAPIData(APIData& data){
    data.getData();
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "Hello World\n");
    DisplayDriver display{DOUT, SCLK, CS, DC, RST, BUSY};
    ESP_LOGI(TAG, "Init Display\n");
    ImageDriver img{IMGWIDTH, IMGHEIGTH};
    ESP_LOGI(TAG, "Init Image\n");
    APIData api{};
    ESP_LOGI(TAG, "Init Graph\n");
    drawLayoutLines(img);
    drawBorderLines(img);
    img.drawImage({236, 30}, {sunnyImg, 128});
    img.drawImage({700, 400}, {sunnyImg, 128});
    ESP_LOGI(TAG, "Image drawn\n");
    img.drawText({32, 32}, "10");
    display.show(img);

    Wifi::init(WIFISSID, WIFIPASS);

    vTaskDelay(pdMS_TO_TICKS(2000));
    drawAPIData(api);
    Wifi::call("http://example.com", [](std::span<const uint8_t> data) {
        ESP_LOGI(TAG, "Got data: %s\n", data.data());
    });

    vTaskDelay(pdMS_TO_TICKS(5000));

    display.clear(img.size());

    display.sleep();
}
