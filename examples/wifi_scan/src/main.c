/*
  This example shows how to scan for available set of Wi-Fi Access Points
  in 2.4 GHz band.

  Xtensa dual-core 32-bit LX6 (ESP32-CAM, FireBeetle ESP32), 240 MHz
  PlatformIO, ESP-IDF framework

  Copyright (c) 2022 Tomas Fryza
  Dept. of Radio Electronics, Brno University of Technology, Czechia
  This example code is in the Public Domain (or CC0 licensed, at your option.)

  Unless required by applicable law or agreed to in writing, this
  software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
  CONDITIONS OF ANY KIND, either express or implied.

  Inspired by:
    * https://github.com/espressif/esp-idf/blob/master/examples/wifi/scan/main/scan.c
    * https://github.com/VALERE91/ESP32_WifiScan/blob/master/main/main.c
    * https://github.com/LetsControltheController/wifi-scan
*/


/*-----------------------------------------------------------*/
#include <esp_log.h>            // ESP_LOG/E/W/I functions
#include <nvs_flash.h>          // Memory
#include <esp_wifi.h>           // Wi-Fi driver
#include <driver/gpio.h>        // GPIO pins


/*-----------------------------------------------------------*/
// On-board LED(s):
// ESP32-CAM  : #33 (red, bottom side), #4 (Flash, top side)
// FireBeetle : #2 (blue)
#define BUILT_IN_LED 2


/*-----------------------------------------------------------*/
// Tag for ESP_LOG/E/W/I functions
static const char *TAG = "wifi scan";


/*-----------------------------------------------------------*/
/* Print authentication mode used by AP */
static void print_auth_mode(int authmode)
{
    switch (authmode) {
        case WIFI_AUTH_OPEN:
            ESP_LOGI(TAG, "Authmode\tWIFI_AUTH_OPEN");
            break;
        case WIFI_AUTH_WEP:
            ESP_LOGI(TAG, "Authmode\tWIFI_AUTH_WEP");
            break;
        case WIFI_AUTH_WPA_PSK:
            ESP_LOGI(TAG, "Authmode\tWIFI_AUTH_WPA_PSK");
            break;
        case WIFI_AUTH_WPA2_PSK:
            ESP_LOGI(TAG, "Authmode\tWIFI_AUTH_WPA2_PSK");
            break;
        case WIFI_AUTH_WPA_WPA2_PSK:
            ESP_LOGI(TAG, "Authmode\tWIFI_AUTH_WPA_WPA2_PSK");
            break;
        case WIFI_AUTH_WPA2_ENTERPRISE:
            ESP_LOGI(TAG, "Authmode\tWIFI_AUTH_WPA2_ENTERPRISE");
            break;
        case WIFI_AUTH_WPA3_PSK:
            ESP_LOGI(TAG, "Authmode\tWIFI_AUTH_WPA3_PSK");
            break;
        case WIFI_AUTH_WPA2_WPA3_PSK:
            ESP_LOGI(TAG, "Authmode\tWIFI_AUTH_WPA2_WPA3_PSK");
            break;
        default:
            ESP_LOGI(TAG, "Authmode\tWIFI_AUTH_UNKNOWN");
            break;
    }
}


/*-----------------------------------------------------------*/
/* Example main */
void app_main(void)
{
    gpio_reset_pin(BUILT_IN_LED);
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Configure and run the scan process in blocking mode
    wifi_scan_config_t scan_conf = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true
    };

    uint16_t ap_num = 0;
    wifi_ap_record_t ap_records[10];

    // Forever loop
    while (1) {
        gpio_set_level(BUILT_IN_LED, 1);
        ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_conf, true));
        gpio_set_level(BUILT_IN_LED, 0);

        // Get number of APs found in last scan
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));

        // Get AP list found in last scan
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));

        // Print the AP list
        ESP_LOGI("---------- Wi-Fi Access Points found", "%u ----------", ap_num);

        for (uint8_t i = 0; i < ap_num; i++) {
            ESP_LOGI(TAG, "SSID \t%s", ap_records[i].ssid);
            ESP_LOGI(TAG, "MAC of AP\t%2x:%2x:%2x:%2x:%2x:%2x",
                *ap_records[i].bssid, *(ap_records[i].bssid+1), *(ap_records[i].bssid+2),
                *(ap_records[i].bssid+3), *(ap_records[i].bssid+4), *(ap_records[i].bssid+5));
            ESP_LOGI(TAG, "RSSI \t%3d dBm", ap_records[i].rssi);
            print_auth_mode(ap_records[i].authmode);
            ESP_LOGI(TAG, "Channel \t%d\n", ap_records[i].primary);
        }

        printf("\n\n");
        
        // Delay 5 seconds
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
