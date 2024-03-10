/**
 * @brief Class for handling Wifi connections
 * @author AntonPieper
 */

#ifndef WIFI_H
#define WIFI_H

#include <functional>
#include <span>
#include <string>
#include <string_view>

#include "esp_http_client.h"

/// @brief Error codes for the Wifi class
enum class CallError { None, NoWifi, NoClient, Failed };

/// @brief Callback type for the Wifi class
using WifiCallback = std::function<void(std::span<const uint8_t>)>;

/// @brief Class for handling Wifi connections
class Wifi {
public:
    /**
     * @brief Initialize the Wifi connection
     *
     * @param ssid The SSID of the Wifi network
     * @param password The password of the Wifi network
     */
    static void init(std::string_view ssid, std::string_view password);
    /**
     * @brief Call a URL and call the callback with the response
     *
     * @param url The URL to call
     * @param callback The callback to call with the response
     * @return The error code
     */
    static CallError call(std::string url, WifiCallback callback);
    volatile static bool established;
};

/// @brief Class for handling a single Wifi call
class WifiCall {
public:
    /**
     * @brief Construct a new Wifi Call object
     *
     * @param url The URL to call
     * @param callback The callback to call with the response
     */
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