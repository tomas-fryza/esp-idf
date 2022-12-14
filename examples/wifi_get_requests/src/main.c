/*
   Create HTTP client on ESP32 and perform GET requests from HTTP site.

   Xtensa dual-core 32-bit LX6 (ESP32-CAM, FireBeetle ESP32), 240 MHz
   PlatformIO, ESP-IDF framework

   Copyright (c) 2022 Tomas Fryza
   Dept. of Radio Electronics, Brno University of Technology, Czechia
   This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.

   TODO:
     Set SSID and password in "include/my_data.h" file

   See also:
     Setup ESP32 as WiFi Station (ESP-IDF)
       * https://embeddedexplorer.com/esp32-wifi-station/

     HTTP Client - FreeRTOS ESP-IDF - GET request
       * https://www.youtube.com/watch?v=2NZgq_pRdN0
       * https://github.com/SIMS-IOT-Devices/FreeRTOS-ESP-IDF-HTTP-Client

     Secure HTTPS site example:
       * https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/protocols/esp_http_client.html

     HTTP + HTTPS Examples:
     * https://github.com/espressif/esp-idf/blob/master/examples/protocols/esp_http_client/main/esp_http_client_example.c
 */


/*-----------------------------------------------------------*/
#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>
#include <esp_log.h>            // ESP_LOG/E/W/I functions
#include <nvs_flash.h>          // Memory
#include <esp_wifi.h>           // Wi-Fi
#include <esp_netif.h>
#include <esp_http_client.h>
#include <my_data.h>


/*-----------------------------------------------------------*/
// Tag for ESP_LOG/E/W/I functions
static const char *TAG = "wifi station";


/*-----------------------------------------------------------*/
void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
/*
{
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WiFi STA connecting...");
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "WiFi connected...");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGW(TAG, "WiFi lost connection");
            break;
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "WiFi got IP...");
            break;
        default:
            break;
    }
}
*/
{
    static uint8_t s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            // xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        // xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


/*-----------------------------------------------------------*/
esp_err_t http_event_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGW(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA: %.*s", evt->data_len, (char *)evt->data);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        default:
            break;
    }
    return ESP_OK;
}


/*-----------------------------------------------------------*/
void wifi_init_sta()
{
    // See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#esp32-wi-fi-station-general-scenario

    // 1 - Wi-Fi/LwIP init phase (LwIP is a lightweight TCP/IP stack)
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
        &event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
        &event_handler, NULL, &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            // .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            // .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // 3 - Wi-Fi Start Phase
    esp_wifi_start();

    // 4- Wi-Fi Connect Phase
    // esp_wifi_connect();

    // Delay 5 seconds
    // vTaskDelay(5000 / portTICK_PERIOD_MS);
}


/*-----------------------------------------------------------*/
void HttpClientTask()
{
    esp_http_client_config_t config = {
        .url = "http://worldclockapi.com/api/json/utc/now",
        // .url = "http://httpbin.org/get",
        // .url = "http://api.thingspeak.com/update?api_key=YOUR_WRITE_API_KEY&field1=YOUR_VALUE",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = http_event_handler
    };

    // Forever loop
    while (1) {
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_perform(client);
        esp_http_client_cleanup(client);

        // Delay 10 seconds
        for (uint8_t i = 10; i > 0; i--) {
            ESP_LOGI(TAG, "%d", i);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    // Delete this task if it exits from the loop above
    vTaskDelete(NULL);
}


/*-----------------------------------------------------------*/
/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main(void)
{
    // Initialize NVS (Non-volatile storage is a partition in flash
    // memory which stores key-value pairs)
    nvs_flash_init();

    // Establish Wi-Fi connection
    wifi_init_sta();

    // Create Wi-Fi task
    xTaskCreate(HttpClientTask, "ESP HTTP Client", 4096, NULL, 5, NULL);
}
