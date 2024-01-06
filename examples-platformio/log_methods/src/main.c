/*
   Logging methods for Serial monitor.
   Xtensa dual-core 32-bit LX6 (ESP32-CAM), 240 MHz
   PlatformIO, ESP-IDF framework

   Copyright (c) 2022 Tomas Fryza
   Dept. of Radio Electronics, Brno University of Technology, Czechia
   This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.
 */


/*-----------------------------------------------------------*/
#include <stdio.h>              // Needed for "printf" function
#include <esp_log.h>            // ESP_LOG/E/W/I functions
#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>      // vTaskDelay, portTICK_PERIOD_MS
#include <driver/gpio.h>        // GPIO pins


/*-----------------------------------------------------------*/
// ESP32-CAM on-board LED(s): #33 (red, bottom side), #4 (Flash, top side)
#define BUILT_IN_LED 33


/*-----------------------------------------------------------*/
/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main()
{
    uint8_t led_state = 0;

    // Standard output to serial monitor
    printf("\nHi, there!\n");

    /* Multiple-level logging methods (first three enabled by default):
        ESP_LOGE -- Error (lowest)
        ESP_LOGW -- Warning
        ESP_LOGI -- Info
        ESP_LOGD -- Debug
        ESP_LEGV -- Verbose (highest)
     */
    ESP_LOGE("setup", "this is Error logging");
    ESP_LOGW("setup", "this is Warning logging");
    ESP_LOGI("setup", "this is Info logging");

    // Pin(s) configuration
    gpio_reset_pin(BUILT_IN_LED);
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);
    ESP_LOGI("gpio", "gpio configured to blink LED");

    // Forever loop
    while (1) {
        led_state = !led_state;                   // Toggle LED
        gpio_set_level(BUILT_IN_LED, led_state);  // Update LED
        ESP_LOGI("gpio", "turning LED %s", led_state == 0 ? "ON" : "OFF");
        vTaskDelay(1000 / portTICK_PERIOD_MS);    // Delay 1 second
    }

    // Will never reach this
}
