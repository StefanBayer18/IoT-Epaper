#include "WifiDriver.h"

#include <cstring>

#include "esp_log.h"
#include "esp_wifi.h"

#define TAG "WifiDriver"

volatile bool Wifi::established = false;

static void wifi_event_handler(void *event_handler_arg,
                               esp_event_base_t event_base, int32_t event_id,
                               void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        Wifi::established = false;
        ESP_LOGI(TAG, "connect to the AP");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        Wifi::established = false;
        ESP_LOGI(TAG, "retry to connect to the AP");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&(event->ip_info.ip)));
        Wifi::established = true;
    } else {
        ESP_LOGI(TAG, "unhandled event (%s) with ID %ld!", event_base,
                 event_id);
    }
}
CallError Wifi::call(std::string url, WifiCallback callback) {
    WifiCall call{std::move(url), std::move(callback)};
    return call();
}
void Wifi::init(std::string_view ssid, std::string_view password) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                               wifi_event_handler, nullptr));
    wifi_config_t wifi_config = {};
    std::memcpy(wifi_config.sta.ssid, ssid.data(),
                std::min(ssid.size(), sizeof(wifi_config.sta.ssid)));
    std::memcpy(wifi_config.sta.password, password.data(),
                std::min(password.size(), sizeof(wifi_config.sta.password)));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(
        static_cast<wifi_interface_t>(ESP_IF_WIFI_STA), &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

WifiCall::WifiCall(std::string url, WifiCallback callback)
    : url(std::move(url)), callback(std::move(callback)) {}

WifiCall::~WifiCall() {
    if (client != nullptr) {
        esp_http_client_cleanup(client);
    }
}

CallError WifiCall::operator()() {
    if (!Wifi::established) {
        ESP_LOGE(TAG, "Wifi not established");
        return CallError::NoWifi;
    }
    esp_http_client_config_t config = {};
    config.url = this->url.c_str();
    config.user_data = this;
    config.event_handler = [](esp_http_client_event_t *evt) {
        if (evt == nullptr) {
            ESP_LOGE(TAG, "Event is null");
            return ESP_FAIL;
        }
        switch (evt->event_id) {
            case HTTP_EVENT_ON_DATA: {
                ESP_LOGI(TAG, "Got data");
                auto call = static_cast<WifiCall *>(evt->user_data);
                if (!call) {
                    ESP_LOGE(TAG, "No user data");
                    return ESP_FAIL;
                }
                call->data.insert(
                    call->data.end(), static_cast<const uint8_t *>(evt->data),
                    static_cast<const uint8_t *>(evt->data) + evt->data_len);
                break;
            }
            case HTTP_EVENT_ON_FINISH: {
                ESP_LOGI(TAG, "Finished");
                auto call = static_cast<WifiCall *>(evt->user_data);
                if (!call) {
                    ESP_LOGE(TAG, "No user data");
                    return ESP_FAIL;
                }
                call->callback(call->data);
                break;
            }
            case HTTP_EVENT_ERROR:
                ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
                break;
            case HTTP_EVENT_ON_CONNECTED:
                ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
                break;
            case HTTP_EVENT_HEADER_SENT:
                ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
                break;
            case HTTP_EVENT_ON_HEADER:
                ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
                break;
            case HTTP_EVENT_DISCONNECTED:
                ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
                break;
            case HTTP_EVENT_REDIRECT:
                ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
                break;
        }
        return ESP_OK;
    };
    client = esp_http_client_init(&config);
    if (client == nullptr) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        return CallError::NoClient;
    }
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to perform HTTP request");
        return CallError::Failed;
    }
    return CallError::None;
}