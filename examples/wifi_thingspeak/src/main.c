/*
  This example shows how to read data from I2C sensor, create HTTP client on ESP32,
  and perform GET requests to ThinngSpeak.

  Xtensa dual-core 32-bit LX6 (FireBeetle ESP32), 240 MHz
  PlatformIO, ESP-IDF framework

  Copyright (c) 2022 Tomas Fryza
  Dept. of Radio Electronics, Brno University of Technology, Czechia
  This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.

  NOTES:
    * Set your Wi-Fi SSID and password in "include/my_data.h"
    * Set your ThingSpeak Write API key in "include/my_data.h"
 */


/*-----------------------------------------------------------*/
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>            // ESP_LOG/E/W/I functions
#include <nvs_flash.h>          // Memory
#include <esp_wifi.h>           // Wi-Fi driver
#include <esp_netif.h>
#include <esp_http_client.h>
#include <my_data.h>
#include <driver/i2c.h>         // Inter-Integrated Circuit driver
#include <stdio.h>              // sprintf() function


/*-----------------------------------------------------------*/
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_ACK 0x0
#define I2C_NACK 0x1
#define I2C_DHT_ADDRESS 0x5c  // DHT12 temp & humidity sensor
#define I2C_DHT_HUMID 0x00    // 0x00 @ Humidity
#define I2C_DHT_TEMP 0x02     // 0x02 @ Temperature

// On-board LED(s):
// FireBeetle : #2 (blue)
#define BUILT_IN_LED 2


/*-----------------------------------------------------------*/
// Tag for ESP_LOG/E/W/I functions
static const char *TAG = "wifi thingspeak";

// DHT12 sensor values
struct DHT12_values_structure {
    uint8_t humidInt;
    uint8_t humidDec;
    uint8_t tempInt;
    uint8_t tempDec;
    uint8_t checksum;
} dht12;


/*-----------------------------------------------------------*/
esp_err_t http_event_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id) {
/*        case HTTP_EVENT_ERROR:
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
*/
        // ThingSpeak return the number of Entries in the channel
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA: %.*s", evt->data_len, (char *)evt->data);
            break;
/*        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
*/
        default:
            break;
    }
    return ESP_OK;
}


/*-----------------------------------------------------------*/
void dht_get_all_values()
{
    // Create and execute i2c commands list
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, I2C_DHT_HUMID, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &dht12.humidInt, I2C_ACK);
    i2c_master_read_byte(cmd, &dht12.humidDec, I2C_ACK);
    i2c_master_read_byte(cmd, &dht12.tempInt, I2C_ACK);
    i2c_master_read_byte(cmd, &dht12.tempDec, I2C_ACK);
    i2c_master_read_byte(cmd, &dht12.checksum, I2C_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, (1000 / portTICK_RATE_MS));
    i2c_cmd_link_delete(cmd);
}


/*-----------------------------------------------------------*/
void thingspeak_task()
{
    char link[100] = "";

    ESP_LOGI(TAG, "ThingSpeak task started");

    sprintf(link, "http://api.thingspeak.com/update?api_key=%s&field1=%d.%d&field2=%d.%d", THINGSPEAK_WRITE_API_KEY, dht12.tempInt, dht12.tempDec, dht12.humidInt, dht12.humidDec);

    // Show request link for debugging purposes
    // ESP_LOGI(TAG, "%s", link);

    esp_http_client_config_t config = {
        .url = link,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = http_event_handler
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);

    // Delete this task
    vTaskDelete(NULL);
}


/*-----------------------------------------------------------*/
void dht_sensor_task()
{
    ESP_LOGI(TAG, "DHT sensor task started");

    // Forever loop
    while (1) {
        // Turn the LED on
        gpio_set_level(BUILT_IN_LED, 1);

        // Read values from I2C sensor
        dht_get_all_values();

        // Send data to ThingSpeak
        xTaskCreate(thingspeak_task, "send_values_to_thingspeak", 4096, NULL, 5, NULL);

        // Turn the LED off
        gpio_set_level(BUILT_IN_LED, 0);

        // Delay 60 seconds
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }

    // Delete this task if it exits from the loop above
    vTaskDelete(NULL);
}


/*-----------------------------------------------------------*/
void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static uint8_t s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < WIFI_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;

        // Delay 5 seconds
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        // Start I2C sensor task
        xTaskCreate(dht_sensor_task, "read_sensor_values", 2048, NULL, 5, NULL);
    }
}


/*-----------------------------------------------------------*/
// Initialize Wi-Fi as STA
void wifi_init_sta()
{
    // See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#esp32-wi-fi-station-general-scenario

    // 1 - Wi-Fi/LwIP init phase (LwIP is a lightweight TCP/IP stack)
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip);

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
    ESP_ERROR_CHECK(esp_wifi_start());
}


/*-----------------------------------------------------------*/
void i2c_setup(void)
{
    // Configure i2c controller in master mode
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    ESP_LOGI(TAG, "i2c controller configured");

    // Install i2c driver
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    ESP_LOGI(TAG, "i2c driver installed");
}


/*-----------------------------------------------------------*/
/* Example main */
void app_main(void)
{
    // GPIO
    gpio_reset_pin(BUILT_IN_LED);
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);

    // I2C
    i2c_setup();

    // Wi-Fi
    // Initialize NVS (Non-volatile storage in Flash memory)
    ESP_ERROR_CHECK(nvs_flash_init());
    // Initialize Wi-Fi connection
    wifi_init_sta();
}
