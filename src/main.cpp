#include <ArduinoJson.h>
#include <DisplayDriver.h>
#include <ImageDriver.h>
#include <WifiDriver.h>

#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sunnyImg.h"

#define CONFIG_ARDUINO_LOOP_STACK_SIZE 16384

#define TAG "MAIN"

enum images { Sunny, SunCloud, Cloudy, Rain, Thunder };

// FiFo Data Storage for Graph
RTC_NOINIT_ATTR uint8_t graphData[GRAPHWIDTH]; // 200 Pixel Height -> 8Bit enough
RTC_NOINIT_ATTR uint16_t start;
RTC_NOINIT_ATTR uint16_t end;

float temp;
float minTemp;
float maxTemp;
float humi;
int imgCode;
int date;
int hour;
int minute;

Image imgCodeToImg(int code) {
    switch (code) {
        case 1000:
            return {sunnyImg, 128};
        case 1003:
            return {cloudyImg, 128};
        default:
            return {cloudImg, 128};
    }
}

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

void addGraphData(float temp){
    temp = std::min(temp, 35.0F);
    temp = std::max(temp, -10.0F); // temp in Wertebereich bringen
    graphData[end] = (uint8_t)((temp + 10.0F) * 4); // In Koordinate Verwandeln (0-180)
    end = (end + 1) % GRAPHWIDTH;
    if(end == start){
        start = (start + 1) % GRAPHWIDTH; //Ältestes Element überschreiben
    }
}

void drawGraph(ImageDriver& img){
    //Displays temp -10 to 35 °C using 180 Pixel Height
    // Pro Grad 4 Pixel
    img.drawLine({250, 430}, {750, 430}); // Nulllinie
    img.drawLine({250, 290},{250, 470}); //Temp Anzeige
    for(unsigned int x = start; (x + 1) % GRAPHWIDTH != end; x = (x + 1) % GRAPHWIDTH){
        img.drawPoint({250+x, (unsigned int)(470-graphData[x])});
        //TODO kontinuierliche Linie erzeugen
    }
}

JsonDocument getAPIData(std::string query) {
    JsonDocument json;
    Wifi::call(query, [&json](std::span<const uint8_t> data) {
        printf("Hello from callback\n\n\n\n");
        ESP_LOGI(TAG, "CALLBACK: %s\n", data.data());
        deserializeJson(json, data.data());
    });
    return json;
}

void drawAPIData(ImageDriver& img) {
    JsonDocument json = getAPIData(APIQUERY);
    // current values
    temp = json["current"]["temp_c"].as<float>();
    humi = json["current"]["humidity"].as<float>();
    imgCode = json["current"]["condition"]["code"].as<int>();
    printf("temp: %0.2f, humi: %0.2f, imgCode: %d\n", temp, humi, imgCode);
    img.drawImage({36, 56}, imgCodeToImg(imgCode));
    img.drawText({70, 200}, std::to_string(temp) + " °c");

    // todays value
    minTemp =
        json["forecast"]["forecastday"][0]["day"]["mintemp_c"].as<float>();
    maxTemp =
        json["forecast"]["forecastday"][0]["day"]["maxtemp_c"].as<float>();
    humi = json["forecast"]["forecastday"][0]["day"]["avghumidity"].as<float>();
    imgCode = json["forecast"]["forecastday"][0]["day"]["condition"]["code"]
                  .as<int>();
    printf("%0.2f - %0.2f\n %0.2f\nImgCode: %d", minTemp, maxTemp, humi,
           imgCode);
    img.drawImage({236, 56}, imgCodeToImg(imgCode));
    img.drawText({270, 200}, std::to_string(minTemp) + " - " +
                                 std::to_string(maxTemp) + "°c");

    // forecast
    minTemp =
        json["forecast"]["forecastday"][1]["day"]["mintemp_c"].as<float>();
    maxTemp =
        json["forecast"]["forecastday"][1]["day"]["maxtemp_c"].as<float>();
    humi = json["forecast"]["forecastday"][1]["day"]["avghumidity"].as<float>();
    imgCode = json["forecast"]["forecastday"][1]["day"]["condition"]["code"]
                  .as<int>();
    printf("%0.2f - %0.2f\n %0.2f\nImgCode: %d", minTemp, maxTemp, humi,
           imgCode);
    img.drawImage({436, 56}, imgCodeToImg(imgCode));
    img.drawText({470, 200}, std::to_string(minTemp) + " - " +
                                 std::to_string(maxTemp) + "°c");

    minTemp =
        json["forecast"]["forecastday"][2]["day"]["mintemp_c"].as<float>();
    maxTemp =
        json["forecast"]["forecastday"][2]["day"]["maxtemp_c"].as<float>();
    humi = json["forecast"]["forecastday"][2]["day"]["avghumidity"].as<float>();
    imgCode = json["forecast"]["forecastday"][2]["day"]["condition"]["code"]
                  .as<int>();
    printf("%0.2f - %0.2f\n %0.2f\nImgCode: %d", minTemp, maxTemp, humi,
           imgCode);
    img.drawImage({636, 56}, imgCodeToImg(imgCode));
    img.drawText({670, 200}, std::to_string(minTemp) + " - " +
                                 std::to_string(maxTemp) + "°c");
    img.drawText({670, 240}, std::to_string(humi) + "%");
}

extern "C" void app_main() {
    //Initialized Graphdata only on powerup
    esp_reset_reason_t reason = esp_reset_reason();
    if ((reason != ESP_RST_DEEPSLEEP) && (reason != ESP_RST_SW)) {
        start = 0;
        end = 1;
        for(int x = 0; x < GRAPHWIDTH; x++){
            graphData[x] = 0;
        }
        printf("Initialized GraphData");
    }

    ESP_LOGI(TAG, "Hello World");
    DisplayDriver display{DOUT, SCLK, CS, DC, RST, BUSY};
    // ESP_LOGI(TAG, "Init Display");
    ImageDriver img{IMGWIDTH, IMGHEIGTH};
    // ESP_LOGI(TAG, "Init Image");
    drawLayoutLines(img);
    drawBorderLines(img);

    Wifi::init(WIFISSID, WIFIPASS);

    vTaskDelay(pdMS_TO_TICKS(5000));

    drawAPIData(img);

    addGraphData(12); // TODO Real value
    drawGraph(img);

    display.show(img);
    vTaskDelay(pdMS_TO_TICKS(5000));

    display.clear(img.size());

    display.sleep();

    printf("Done with the Code.\nGoing to sleep now.\n");
    esp_sleep_enable_timer_wakeup(300 * 1000000);
    esp_deep_sleep_start();
}
