/**
 * @brief Configuration file for the weather station
 * @author Stefan
 */

#ifndef CONFIG
#define CONFIG

// Display Pins
#define DOUT GPIO_NUM_13
#define SCLK GPIO_NUM_14
#define CS GPIO_NUM_15
#define DC GPIO_NUM_27
#define RST GPIO_NUM_26
#define BUSY GPIO_NUM_25

// Image Dimensions
#define IMG_WIDTH 800
#define IMG_HEIGTH 480
#define GRAPH_WIDTH 500

// Wifi
#define WIFI_SSID "EMPTY"
#define WIFI_PASS "EMPTY"

// Database
#define OUTSIDE_URL "http://192.168.188.53/outside.php"
#define INDOOR_URL "http://192.168.188.53/room.php"

#define SLEEP_TIMER 15
// API
#define API_QUERY                                            \
    "http://api.weatherapi.com/v1/"                          \
    "forecast.json?key=PLACEAPIKEYHERE&q=PLACEYOURPLACEHERE" \
    "&days=3&aqi=no&alerts=no"

#endif