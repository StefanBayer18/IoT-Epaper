#include <ArduinoJson.h>
#include <DisplayDriver.h>
#include <ImageDriver.h>
#include <WifiDriver.h>

#include <optional>

#include "Images.h"
#include "config.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "MAIN"

// FiFo Data Storage for Graph
RTC_NOINIT_ATTR uint8_t
    graphData[GRAPHWIDTH];  // 200 Pixel Height -> 8Bit enough
RTC_NOINIT_ATTR uint16_t start;
RTC_NOINIT_ATTR uint16_t end;

enum MeasurementIndex {
    DataToday,
    DataTomorrow,
    DataDayAfterTomorrow,
    DataCurrent,
    DataInside,
    DataOutside,
    DataIndexCount
};

RTC_NOINIT_ATTR struct Data {
    float temp;
    float minTemp;
    float maxTemp;
    float humi;
    int imgCode;
    int date;
    int hour;
    int minute;
} measurements[DataIndexCount];

const std::string weekdays[] = {"Montag",  "Dienstag", "Mittwoch", "Donnerstag",
                                "Freitag", "Samstag",  "Sonntag"};

/**
 * @brief Converts an Code given by the API into an image
 * @param code The code of the image
 * @return An image
 */
Image imgCodeToImg(int code) {
    switch (code) {
        case 1000:
            return {sunnyImg, 128};
        case 1003:
        case 1150:
        case 1204:
            return {cloudyImg, 128};
        case 1006:
        case 1009:
        case 1030:
        case 1069:
        case 1072:
        case 1135:
        case 1168:
        case 1249:
            return {cloudImg, 128};
        case 1087:
            return {thunderImg, 128};
        case 1114:
        case 1117:
        case 1147:
        case 1153:
        case 1171:
        case 1201:
        case 1066:
        case 1222:
        case 1237:
        case 1252:
        case 1255:
        case 1258:
        case 1261:
        case 1264:
            return {snowImg, 128};
        case 1180:
        case 1183:
        case 1186:
        case 1189:
        case 1192:
        case 1195:
        case 1198:
        case 1207:
        case 1210:
        case 1213:
        case 1216:
        case 1219:
        case 1225:
        case 1240:
        case 1243:
        case 1246:
        case 1063:
            return {rainImg, 128};
        case 1273:
        case 1276:
        case 1279:
        case 1282:
            return {thunderImg, 128};
        default:
            return {cloudImg, 128};
    }
}

void drawWeekdays(ImageDriver& img) {
    img.drawCenteredText({100, 35}, "Momentan");
    auto& data = measurements[DataInside];
    img.drawCenteredText({300, 35}, weekdays[data.date]);
    img.drawCenteredText({500, 35}, weekdays[(data.date + 1) % 7]);
    img.drawCenteredText({700, 35}, weekdays[(data.date + 2) % 7]);
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

void addGraphData(float temp) {
    temp = std::min(temp, 35.0F);
    temp = std::max(temp, -10.0F);  // temp in Wertebereich bringen
    graphData[end] =
        (uint8_t)((temp + 10.0F) * 4);  // In Koordinate Verwandeln (0-180)
    end = (end + 1) % GRAPHWIDTH;
    if (end == start) {
        start = (start + 1) % GRAPHWIDTH;  // Ältestes Element überschreiben
    }
}

// draws an 2x2 Square at given position
void drawRect(ImageDriver& img, Vec2u pos) {
    img.drawPoint({pos.x, pos.y});
    img.drawPoint({pos.x + 1, pos.y});
    img.drawPoint({pos.x, pos.y + 1});
    img.drawPoint({pos.x + 1, pos.y + 1});
}

void drawGraph(ImageDriver& img) {
    // Displays temp -10 to 35 °C using 180 Pixel Height
    //  Pro Grad 4 Pixel
    img.drawFilledRect({240, 428}, {510, 2});  // Nulllinie
    img.drawFilledRect({248, 290}, {2, 200});  // Temp Anzeige

    float prev = graphData[start];
    int size = 0;
    for (unsigned int x = start; x != end; x = (x + 1) % GRAPHWIDTH) {
        size++;
        drawRect(img, {250 + x, (unsigned int)(470 - graphData[x])});
        int diff = std::abs(graphData[x] - prev);
        if (diff <= 1) continue;

        int dir = prev > graphData[x] ? -1 : 1;
        for (int i = 0; i < diff; i++) {
            drawRect(img, {250 + (i >= (diff / 2) ? x : (x - 1)),
                           (unsigned int)(470 - (prev + i * dir))});
        }
        prev = graphData[x];
    }
    // Achsenbeschriftung
    img.drawCenteredText({220, 455}, "-10");
    img.drawFilledRect({240, 469}, {9, 2});

    img.drawText({225, 418}, "0");

    img.drawCenteredText({220, 280}, "30");
    img.drawFilledRect({240, 290}, {9, 2});
    img.drawCenteredText({265, 285}, "°c");

    // Drawing Daylines
    int measuresPerHour = 60 / sleepTimer;
    auto& data = measurements[DataInside];
    int offset = (data.minute / sleepTimer) + (data.hour * measuresPerHour);
    size -= offset;
    while (size > 0) {
        img.drawFilledRect({(unsigned int)(250 + size), 420}, {2, 20});
        size -= measuresPerHour * 24;
    }
}

std::optional<JsonDocument> getAPIData(const std::string& query) {
    JsonDocument json;

    int trys = 5;
    CallError call;
    DeserializationError err;
    for (int x = 0; x < trys; x++) {
        call = Wifi::call(query, [&json, &err](std::span<const uint8_t> data) {
            err = deserializeJson(json, data.data());
        });
        if (call == CallError::None && err == DeserializationError::Ok) {
            return std::optional{json};
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return std::nullopt;
}

void drawDatabaseData(ImageDriver& img) {
    bool updateData = true;
    static auto urls =
        std::array{std::string(OUTSIDEURL), std::string(INDOORURL)};
    static auto dataIndex = std::array{DataOutside, DataInside};
    char buffer[64];
    for (size_t i = 0; i < urls.size(); ++i) {
        auto answer = getAPIData(urls[i]);
        if (!answer.has_value()) {
            updateData = false;
        }
        JsonDocument json = *answer;
        auto& data = measurements[dataIndex[i]];
        if (updateData) {
            data.temp = json[0]["Temp"].as<float>();
            data.humi = json[0]["Humidity"].as<float>();
            data.date = json[0]["Date"].as<int>();
            data.hour = json[0]["Hour"].as<int>();
            data.minute = json[0]["Minute"].as<int>();
        }
        std::sprintf(buffer, "%5.2f °c", data.temp);
        img.drawCenteredText({100, 280 + i * 110}, {buffer, 9});
        std::sprintf(buffer, "%5.2f %%", data.humi);
        img.drawCenteredText({100, 320 + i * 110}, {buffer, 7});
    }
    img.drawFilledRect({33, 365}, {134, 3});
}

void drawAPIData(ImageDriver& img) {
    printf("Getting API Data\n");
    bool updateData = true;
    char buffer[64];
    auto answer = getAPIData(APIQUERY);
    if (!answer.has_value()) {
        // Display old Data
        updateData = false;
    }
    JsonDocument json = *answer;
    // current values
    auto& current = measurements[DataCurrent];
    if (updateData) {
        current.temp = json["current"]["temp_c"].as<float>();
        current.humi = json["current"]["humidity"].as<float>();
        current.imgCode = json["current"]["condition"]["code"].as<int>();
    }
    img.drawImage({36, 56}, imgCodeToImg(current.imgCode));
    std::sprintf(buffer, "%5.2f °c", current.temp);
    img.drawCenteredText({100, 200}, {buffer, 9});
    std::sprintf(buffer, "%5.2f %%", current.humi);
    img.drawCenteredText({100, 240}, {buffer, 7});
    for (size_t i = 0; i < 3; ++i) {
        // forecast and today value
        auto& day = measurements[i];
        if (updateData) {
            day.minTemp = json["forecast"]["forecastday"][i]["day"]["mintemp_c"]
                              .as<float>();
            day.maxTemp = json["forecast"]["forecastday"][i]["day"]["maxtemp_c"]
                              .as<float>();
            day.humi = json["forecast"]["forecastday"][i]["day"]["avghumidity"]
                           .as<float>();
            day.imgCode =
                json["forecast"]["forecastday"][i]["day"]["condition"]["code"]
                    .as<int>();
        }
        std::sprintf(buffer, "%5.2f - %5.2f °c", day.minTemp, day.maxTemp);
        Vec2u offset = {200 * i, 0};
        img.drawImage(Vec2u{236, 56} + offset, imgCodeToImg(day.imgCode));
        img.drawCenteredText(Vec2u{300, 200} + offset, {buffer, 17});
        std::sprintf(buffer, "%5.2f %%", day.humi);
        img.drawCenteredText(Vec2u{300, 240} + offset, {buffer, 7});
    }
}

extern "C" void app_main() {
    // Initialized Graphdata only on powerup
    esp_reset_reason_t reason = esp_reset_reason();
    if ((reason != ESP_RST_DEEPSLEEP) && (reason != ESP_RST_SW)) {
        start = 0;
        end = 0;
        memset(graphData, 0, GRAPHWIDTH);
        for (auto& data : measurements) {
            data.temp = NAN;
            data.minTemp = NAN;
            data.maxTemp = NAN;
            data.humi = NAN;
            data.imgCode = -1;
            data.date = -1;
            data.hour = -1;
            data.minute = -1;
        }
    }
    ESP_LOGI(TAG, "Hello World");
    DisplayDriver display{DOUT, SCLK, CS, DC, RST, BUSY};
    // ESP_LOGI(TAG, "Init Display");
    ImageDriver img{IMGWIDTH, IMGHEIGTH};
    // ESP_LOGI(TAG, "Init Image");
    drawLayoutLines(img);
    drawBorderLines(img);

    Wifi::init(WIFISSID, WIFIPASS);
    while (!Wifi::established) {
        vTaskDelay(pdMS_TO_TICKS(100));
        printf(".");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    drawAPIData(img);
    vTaskDelay(pdMS_TO_TICKS(1000));
    drawDatabaseData(img);
    drawWeekdays(img);

    addGraphData(measurements[DataOutside].temp);

    drawGraph(img);

    display.show(img);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // display.clear(img.size());

    display.sleep();
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("Done with the Code.\nGoing to sleep now.\n");
    esp_sleep_enable_timer_wakeup(sleepTimer * 60 * 1000000);
    esp_deep_sleep_start();
}
