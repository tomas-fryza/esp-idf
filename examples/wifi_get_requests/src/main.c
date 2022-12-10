/*
   Create HTTP client on ESP32 and perform GET requests to HTTP site.

   Xtensa dual-core 32-bit LX6 (ESP32-CAM), 240 MHz
   PlatformIO, ESP-IDF framework

   Copyright (c) 2022 Tomas Fryza
   Dept. of Radio Electronics, Brno University of Technology, Czechia
   This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.

   See also:
     HTTP Client - FreeRTOS ESP IDF - GET
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
static const char *TAG = "wifi station";


/*-----------------------------------------------------------*/
// Used function(s)
void wifi_connect();
void event_handler(void *, esp_event_base_t, int32_t, void *);
void vTaskHttpRequest();
esp_err_t http_event_handler(esp_http_client_event_handle_t);


/*-----------------------------------------------------------*/
/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main(void)
{
    // Initialize NVS
    // nvs_flash_init()
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    // Establish Wi-Fi connection
    wifi_connect();
    ESP_LOGI(TAG, "Connected to AP, begin http example");

    // Create Wi-Fi task
    xTaskCreate(vTaskHttpRequest, "http_get_task", 4096, NULL, 5, NULL);
}


/*-----------------------------------------------------------*/
void wifi_connect()
{
    // 1 - Wi-Fi/LwIP Init Phase
    ESP_ERROR_CHECK(esp_netif_init());    // TCP/IP initiation     s1.1
    ESP_ERROR_CHECK(esp_event_loop_create_default());// event loop s1.2

    esp_netif_create_default_wifi_sta();  // WiFi station          s1.3
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);                  //                       s1.4

    // 2 - Wi-Fi Configuration Phase
    // esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    // esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &event_handler,
            NULL,
            &instance_any_id));
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &event_handler,
            NULL,
            &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID,
            .password = PASS,
            // .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            // .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    // esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // 3 - Wi-Fi Start Phase
    esp_wifi_start();

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();

    // Delay 5 seconds
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}


/*-----------------------------------------------------------*/
void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
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


/*-----------------------------------------------------------*/
void vTaskHttpRequest()
{
    ESP_LOGI(TAG, "Start http_request example");

    esp_http_client_config_t config_get_request = {
        .url = "http://worldclockapi.com/api/json/utc/now",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = http_event_handler
    };

    // Forever loop
    while (1) {
        esp_http_client_handle_t client = esp_http_client_init(&config_get_request);
        esp_http_client_perform(client);
        esp_http_client_cleanup(client);

        // Delay 20 seconds
        vTaskDelay(20000 / portTICK_PERIOD_MS);
    }

    // Delete this task if it exits from the loop above
    ESP_LOGI(TAG, "Finish http_request example");
    vTaskDelete(NULL);
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
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED\n");
            break;
        default:
            break;
    }
    return ESP_OK;
}
