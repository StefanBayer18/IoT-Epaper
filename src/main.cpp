/**
 * @brief Main file for the weather station
 * @author AntonPieper, Stefan
 */

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

// Ring buffer for graph
RTC_NOINIT_ATTR uint8_t
    graphData[GRAPH_WIDTH];  // 180 Pixel height -> 8 bit enough
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
 * @brief Converts a code given by the API into an image
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
            ESP_LOGE(TAG, "Unknown Code: %d", code);
            return {cloudImg, 128};
    }
}

/**
 * @brief Draws the current weekday and the next two days
 *
 * @param img The ImageDriver to draw on
 */
void drawWeekdays(ImageDriver& img) {
    auto& data = measurements[DataInside];

    // Is the date valid?
    bool has_date = data.date != -1;

    // Get the weekday names, fallback to "heute", "morgen" and "in 2 tagen"

    std::string_view today = has_date ? weekdays[data.date] : "heute";
    std::string_view tomorrow =
        has_date ? weekdays[(data.date + 1) % 7] : "morgen";
    std::string_view dayAfterTomorrow =
        has_date ? weekdays[(data.date + 2) % 7] : "in 2 tagen";

    img.drawCenteredText({100, 35}, "Momentan");
    img.drawCenteredText({300, 35}, today);
    img.drawCenteredText({500, 35}, tomorrow);
    img.drawCenteredText({700, 35}, dayAfterTomorrow);
}

/**
 * @brief Draws the layout lines for the image
 *
 * @param driver The ImageDriver to draw on
 */
void drawLayoutLines(ImageDriver& img) {
    // Vertical Lines
    img.drawFilledRect({198, 0},
                       {4, img.height()});  // Left (goes to the very bottom)
    img.drawFilledRect({398, 0}, {4, img.height() - 198});  // Middle
    img.drawFilledRect({598, 0}, {4, img.height() - 198});  // Right

    // Horizontal Line for dividing the forecast and the weather graph
    img.drawFilledRect({198, img.height() - 202}, {img.width() - 198, 4});
}

/**
 * @brief Draws the border lines for the display
 *
 * @param driver The ImageDriver to draw on
 */
void drawBorderLines(ImageDriver& img) {
    // Vertical
    img.drawFilledRect({0, 0}, {4, img.height()});                // Left
    img.drawFilledRect({img.width() - 4, 0}, {4, img.height()});  // Right

    // Horizontal
    img.drawFilledRect({0, 0}, {img.width(), 4});                 // Top
    img.drawFilledRect({0, img.height() - 4}, {img.width(), 4});  // Bottom
}

/**
 * @brief Adds a new temperature to the graph
 *
 * @param temp The temperature to add
 */
void addGraphData(float temp) {
    temp = std::min(temp, 35.0F);
    // bring temp in range -10 to 35 °C
    temp = std::max(temp, -10.0F);
    // store graph coordinates in range 0-180
    graphData[end] = (uint8_t)((temp + 10.0F) * 4);
    // move end (and start) to next position
    end = (end + 1) % GRAPH_WIDTH;
    if (end == start) {
        start = (start + 1) % GRAPH_WIDTH;
    }
}

/**
 * @brief Draws a 2x2 rectangle on the image
 *
 * @param img The ImageDriver to draw on
 * @param pos The Position to draw the rectangle
 */
void drawRect(ImageDriver& img, Vec2u pos) {
    img.drawPoint({pos.x, pos.y});
    img.drawPoint({pos.x + 1, pos.y});
    img.drawPoint({pos.x, pos.y + 1});
    img.drawPoint({pos.x + 1, pos.y + 1});
}

/**
 * @brief Draws the graph on the image by connecting the points.
 *
 * @param img The ImageDriver to draw on
 */
void drawGraph(ImageDriver& img) {
    // Displays temp -10 to 35 °C using 180 Pixel Height
    //  4 pixels per °C
    img.drawFilledRect({240, 428}, {510, 2});  // time line (x-axis)
    img.drawFilledRect({248, 290}, {2, 200});  // temp line (y-axis)

    // Drawing Graph data by drawing 2 vertical lines between the points split
    // in the middle
    uint8_t prev = graphData[start];
    int size = 0;
    unsigned offsetY = 470;
    unsigned offsetX = 250;
    for (unsigned x = start; x != end; x = (x + 1) % GRAPH_WIDTH) {
        size++;
        int diff = std::abs(graphData[x] - prev);
        if (diff <= 1) {
            drawRect(img, {offsetX + x, offsetY - graphData[x]});
            continue;
        }

        int dirY = prev > graphData[x] ? -1 : 1;
        for (int i = 0; i < diff; i++) {
            bool right = i >= (diff / 2);
            drawRect(img, {offsetX + (right ? x : (x - 1)),
                           offsetY - (prev + i * dirY)});
        }
        prev = graphData[x];
    }

    // axis labeling
    img.drawCenteredText({220, 455}, "-10");
    img.drawFilledRect({240, 469}, {9, 2});  // small line at -10°C
    img.drawText({225, 418}, "0");

    img.drawCenteredText({220, 280}, "30");
    img.drawFilledRect({240, 290}, {9, 2});  // small line at 30°C
    img.drawCenteredText({265, 285}, "°c");

    // Drawing day lines
    int measuresPerHour = 60 / SLEEP_TIMER;
    auto& data = measurements[DataInside];

    // don't draw if date is invalid
    if (data.date == -1) {
        return;
    }

    int offset = (data.minute / SLEEP_TIMER) + (data.hour * measuresPerHour);
    size -= offset;
    while (size > 0) {
        img.drawFilledRect({(unsigned int)(250 + size), 420}, {2, 20});
        size -= measuresPerHour * 24;
    }
}

/**
 * @brief Calls an API and returns the JSON data
 *
 * @param query The query to call
 * @return The JSON data or nothing if the call failed
 */
std::optional<JsonDocument> getAPIData(const std::string& url) {
    JsonDocument json;

    int tries = 5;
    CallError call;
    DeserializationError err;
    for (int x = 0; x < tries; x++) {
        call = Wifi::call(url, [&](std::span<const uint8_t> data) {
            err = deserializeJson(json, data.data());
        });
        if (call == CallError::None && err == DeserializationError::Ok) {
            return std::optional{json};
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return {};
}

/**
 * @brief Draws the weather data from our database onto the image
 *
 * @param img The ImageDriver to draw on
 */
void drawDatabaseData(ImageDriver& img) {
    static auto urls =
        std::array{std::string(OUTSIDE_URL), std::string(INDOOR_URL)};
    static auto dataIndex = std::array{DataOutside, DataInside};
    char buffer[64];
    for (size_t i = 0; i < urls.size(); ++i) {
        auto& data = measurements[dataIndex[i]];

        // Retrieve the newest data
        auto answer = getAPIData(urls[i]);
        // Overwrite data if we got a new answer
        if (auto json = *answer; answer) {
            data.temp = json[0]["Temp"].as<float>();
            data.humi = json[0]["Humidity"].as<float>();
            data.date = json[0]["Date"].as<int>();
            data.hour = json[0]["Hour"].as<int>();
            data.minute = json[0]["Minute"].as<int>();
        }

        // Draw the data
        std::sprintf(buffer, "%5.2f °c", data.temp);
        img.drawCenteredText({100, 280 + i * 110}, {buffer, 9});
        std::sprintf(buffer, "%5.2f %%", data.humi);
        img.drawCenteredText({100, 320 + i * 110}, {buffer, 7});
    }
    img.drawFilledRect({33, 365}, {134, 3});

    addGraphData(measurements[DataOutside].temp);
}

/**
 * @brief Draws the weather data from a public weather API onto the image
 *
 * @param img The ImageDriver to draw on
 */
void drawAPIData(ImageDriver& img) {
    // current values
    auto& current = measurements[DataCurrent];

    ESP_LOGI(TAG, "Getting API Data");
    auto answer = getAPIData(API_QUERY);
    auto json = *answer;
    if (answer.has_value()) {
        current.temp = json["current"]["temp_c"].as<float>();
        current.humi = json["current"]["humidity"].as<float>();
        current.imgCode = json["current"]["condition"]["code"].as<int>();
    }

    // Draw the weather icon
    img.drawImage({36, 56}, imgCodeToImg(current.imgCode));

    // Buffer for sprintf
    char buffer[64];
    std::sprintf(buffer, "%5.2f °c", current.temp);
    img.drawCenteredText({100, 200}, {buffer, 9});

    std::sprintf(buffer, "%5.2f %%", current.humi);
    img.drawCenteredText({100, 240}, {buffer, 7});

    for (size_t i = 0; i < 3; ++i) {
        // forecast and today value
        auto& day = measurements[i];
        if (answer.has_value()) {
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

        Vec2u offset = {200 * i, 0};

        // Draw the weather icon
        img.drawImage(Vec2u{236, 56} + offset, imgCodeToImg(day.imgCode));

        std::sprintf(buffer, "%5.2f - %5.2f °c", day.minTemp, day.maxTemp);
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
        memset(graphData, 0, GRAPH_WIDTH);
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

    DisplayDriver display{DOUT, SCLK, CS, DC, RST, BUSY};

    ImageDriver img{IMG_WIDTH, IMG_HEIGTH};

    drawLayoutLines(img);
    drawBorderLines(img);

    Wifi::init(WIFI_SSID, WIFI_PASS);

    ESP_LOGI(TAG, "Waiting for Wifi to be established");
    while (!Wifi::established) {
        vTaskDelay(pdMS_TO_TICKS(100));
        printf(".");
    }
    printf("\nWifi established.\n");

    drawAPIData(img);
    drawDatabaseData(img);  // Also adds data to the graph
    drawWeekdays(img);

    drawGraph(img);

    display.show(img);
    vTaskDelay(pdMS_TO_TICKS(100));

    // display.clear(img.size());

    display.sleep();
    printf("Done with the Code.\nGoing to sleep now.\n");
    uint64_t us_per_minute = 60 * 1000000;
    esp_sleep_enable_timer_wakeup(SLEEP_TIMER * us_per_minute);
    esp_deep_sleep_start();
}
