#ifndef WIFI_H
#define WIFI_H

#include <functional>
#include <span>
#include <string>
#include <string_view>

#include "esp_http_client.h"

enum class CallError { None, NoWifi, NoClient, Failed };

using WifiCallback = std::function<void(std::span<const uint8_t>)>;

class Wifi {
public:
    static void init(std::string_view ssid, std::string_view password);
    static CallError call(std::string url, WifiCallback callback);
    volatile static bool established;

private:
    static bool is_initialized;
};

class WifiCall {
public:
    WifiCall(std::string url, WifiCallback callback);

    ~WifiCall();

    CallError operator()();

private:
    std::string url;
    std::vector<uint8_t> data;
    WifiCallback callback;
    esp_http_client_handle_t client = nullptr;
};

#endif  // WIFI_H