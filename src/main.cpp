#include <ArduinoJson.h>
#include <DisplayDriver.h>
#include <ImageDriver.h>
#include <WifiDriver.h>

#include <optional>

#include "config.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Images.h"

#define CONFIG_ARDUINO_LOOP_STACK_SIZE 16384

#define TAG "MAIN"

enum images { Sunny, SunCloud, Cloudy, Rain, Thunder };

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

Image imgCodeToImg(int code) {
    switch (code) {
        case 1000: return {sunnyImg, 128};
        case 1003: return {cloudyImg, 128};
        case 1006: return {cloudImg, 128};
        case 1009: return {cloudImg, 128};
        case 1030: return {cloudImg, 128};
        case 1063: return {rainImg, 128};
        case 1066: return {snowImg, 128};
        case 1069: return {cloudImg, 128};
        case 1072: return {cloudImg, 128};
        case 1087: return {thunderImg, 128};
        case 1114: return {snowImg, 128};
        case 1117: return {snowImg, 128};
        case 1135: return {cloudImg, 128};
        case 1147: return {snowImg, 128};
        case 1150: return {cloudyImg, 128};
        case 1153: return {cloudImg, 128};
        case 1168: return {cloudImg, 128};
        case 1171: return {snowImg, 128};
        case 1180: return {rainImg, 128};
        case 1183: return {rainImg, 128};
        case 1186: return {rainImg, 128};
        case 1189: return {rainImg, 128};
        case 1192: return {rainImg, 128};
        case 1195: return {rainImg, 128};
        case 1198: return {rainImg, 128};
        case 1201: return {snowImg, 128};
        case 1204: return {cloudyImg, 128};
        case 1207: return {rainImg, 128};
        case 1210: return {snowImg, 128};
        case 1213: return {snowImg, 128};
        case 1216: return {snowImg, 128};
        case 1219: return {snowImg, 128};
        case 1222: return {snowImg, 128};
        case 1225: return {snowImg, 128};
        case 1237: return {snowImg, 128};
        case 1240: return {rainImg, 128};
        case 1243: return {rainImg, 128};
        case 1246: return {rainImg, 128};
        case 1249: return {cloudImg, 128};
        case 1252: return {snowImg, 128};
        case 1255: return {snowImg, 128};
        case 1258: return {snowImg, 128};
        case 1261: return {snowImg, 128};
        case 1264: return {snowImg, 128};
        case 1273: return {thunderImg, 128};
        case 1276: return {thunderImg, 128};
        case 1279: return {thunderImg, 128};
        case 1282: return {thunderImg, 128};
        default: return {cloudImg, 128};
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

// draws an 2x2 Aquare at given position
void drawRect(ImageDriver& img, Vec2u pos){
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

    for (unsigned int x = start; x != end; x = (x + 1) % GRAPHWIDTH) {
        printf("X: %d; Y: %d, Val: %d\n", 250 + x,
               (unsigned int)(470 - graphData[x]), graphData[x]);
        drawRect(img, {250 + x, (unsigned int)(470 - graphData[x])});
        // TODO kontinuierliche Linie erzeugen
    }
    // Achsenbeschriftung
    img.drawCenteredText({220, 455}, "-10");
    img.drawFilledRect({240, 469}, {9, 2});

    img.drawText({225, 418}, "0");
    
    img.drawCenteredText({220, 280}, "30");
    img.drawFilledRect({240, 290}, {9, 2});
    img.drawCenteredText({265, 285}, "°c");
    //img.drawText({760, 417}, "Date");

    //TODO Tagesmarker
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
    static auto urls =
        std::array{std::string(OUTSIDEURL), std::string(INDOORURL)};
    static auto dataIndex = std::array{DataOutside, DataInside};
    char buffer[64];
    for (size_t i = 0; i < urls.size(); ++i) {
        auto answer = getAPIData(urls[i]);
        if (!answer.has_value()) {
            return;
        }
        JsonDocument json = *answer;
        auto& data = measurements[dataIndex[i]];
        data.temp = json[0]["Temp"].as<float>();
        data.humi = json[0]["Humidity"].as<float>();
        std::sprintf(buffer, "%5.2f °c", data.temp);
        img.drawCenteredText({100, 280 + i * 110}, {buffer, 8});
        std::sprintf(buffer, "%5.2f%%", data.humi);
        img.drawCenteredText({100, 320 + i * 110}, {buffer, 6});
        data.date = json[0]["Date"].as<int>();
        data.hour = json[0]["Hour"].as<int>();
        data.minute = json[0]["Minute"].as<int>();
    }
    img.drawFilledRect({33, 365}, {134, 3});
}

void drawAPIData(ImageDriver& img) {
    printf("Getting API Data\n");
    auto answer = getAPIData(APIQUERY);
    if (!answer.has_value()) {
        return;
    }
    JsonDocument json = *answer;
    // current values
    auto& current = measurements[DataCurrent];
    current.temp = json["current"]["temp_c"].as<float>();
    current.humi = json["current"]["humidity"].as<float>();
    current.imgCode = json["current"]["condition"]["code"].as<int>();
    //printf("temp: %0.2f, humi: %0.2f, imgCode: %d\n", current.temp, current.humi, current.imgCode);
    img.drawImage({36, 56}, imgCodeToImg(current.imgCode));
    char buffer[64];
    std::sprintf(buffer, "%5.2f °c", current.temp);
    img.drawCenteredText({100, 200}, {buffer, 8});
    std::sprintf(buffer, "%5.2f%%", current.humi);
    img.drawCenteredText({100, 240}, {buffer, 6});
    addGraphData(current.temp);
    for (size_t i = 0; i < 3; ++i) {
        // forecast and today value
        auto& day = measurements[i];
        day.minTemp =
            json["forecast"]["forecastday"][i]["day"]["mintemp_c"].as<float>();
        day.maxTemp =
            json["forecast"]["forecastday"][i]["day"]["maxtemp_c"].as<float>();
        day.humi = json["forecast"]["forecastday"][i]["day"]["avghumidity"]
                       .as<float>();
        day.imgCode =
            json["forecast"]["forecastday"][i]["day"]["condition"]["code"]
                .as<int>();
        //printf("%0.2f - %0.2f C;  %0.2f %%; ImgCode: %d\n", day.minTemp, day.maxTemp, day.humi, day.imgCode);
        std::sprintf(buffer, "%5.2f - %5.2f °c", day.minTemp, day.maxTemp);
        Vec2u offset = {200 * i, 0};
        img.drawImage(Vec2u{236, 56} + offset, imgCodeToImg(day.imgCode));
        img.drawCenteredText(Vec2u{300, 200} + offset, {buffer, 16});
        std::sprintf(buffer, "%5.2f%%", day.humi);
        img.drawCenteredText(Vec2u{300, 240} + offset, {buffer, 6});
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
    while(!Wifi::established){
        vTaskDelay(pdMS_TO_TICKS(100));
        printf(".");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    drawAPIData(img);
    vTaskDelay(pdMS_TO_TICKS(1000));
    drawDatabaseData(img);
    drawWeekdays(img);

    drawGraph(img);

    //img.drawText({450, 378}, std::to_string(end));

    display.show(img);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // display.clear(img.size());

    display.sleep();
    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("Done with the Code.\nGoing to sleep now.\n");
    esp_sleep_enable_timer_wakeup(450 * 1000000);
    esp_deep_sleep_start();
}
