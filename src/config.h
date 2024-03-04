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
#define OUTSIDEURL "http://192.168.188.53/outside.php"
#define INDOORURL "http://192.168.188.53/room.php"

// API
#define APIQUERY "http://api.weatherapi.com/v1/forecast.json?key=PLACEAPIKEYHERE&q=Bad Hersfeld&days=3&aqi=no&alerts=no"

#endif