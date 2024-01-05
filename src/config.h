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
#define IMGWIDTH 800
#define IMGHEIGTH 480
#define GRAPHWIDTH 500

// Wifi
#define WIFISSID "EMPTY"
#define WIFIPASS "EMPTY"

// Database
#define DATABASEUSER "WeatherDisplay"
#define DATABASEPASS "EINKDISPLAY"
#define DATABASEDATA "Discord"
#define DATABASETABLE "OUTSIDE"

// API
#define APIQUERY "http://api.weatherapi.com/v1/current.json?key=
#define APIKEY "EMPTY"
#define APIPARAM "&q=Bad Hersfeld&aqi=no"

#endif