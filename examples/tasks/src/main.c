/*
   Example with two tasks.
   Xtensa dual-core 32-bit LX6 (ESP32-CAM), 240 MHz
   PlatformIO, ESP-IDF framework

   Copyright (c) 2022 Tomas Fryza
   Dept. of Radio Electronics, Brno University of Technology, Czechia
   This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.
 */


/*-----------------------------------------------------------*/
#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>      // vTaskDelay, portTICK_PERIOD_MS
#include <esp_log.h>            // ESP_LOG/E/W/I functions
#include <esp_random.h>         // Random number generation
#include <driver/gpio.h>        // GPIO pins


/*-----------------------------------------------------------*/
// ESP32-CAM on-board LED(s): #33 (red, bottom side), #4 (Flash, top side)
#define BUILT_IN_LED 33


/*-----------------------------------------------------------*/
// Used function(s)
void vTaskBlink();
void vTaskRandom();


/*-----------------------------------------------------------*/
/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main()
{
    ESP_LOGI("setup", "task application");

    xTaskCreate(vTaskBlink, "Task 1", 2048, NULL, 5, NULL);
    xTaskCreate(vTaskRandom, "Task 2", 2048, NULL, 5, NULL);
}


/*-----------------------------------------------------------*/
void vTaskBlink()
{
    uint8_t led_state = 0;

    // Pin(s) configuration
    gpio_reset_pin(BUILT_IN_LED);
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);
    ESP_LOGI("task 1", "gpio configured to blink LED");

    // Forever loop
    while (1) {
        // Toggle LED
        led_state = !led_state;    
        gpio_set_level(BUILT_IN_LED, led_state);
        ESP_LOGI("task 1", "turning LED %s", led_state == 0 ? "ON" : "OFF");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // Delete this task if it exits from the loop above
    vTaskDelete(NULL);
}


/*-----------------------------------------------------------*/
void vTaskRandom()
{
    uint16_t cnt = 0;
    uint16_t randomDelay;

    // Forever loop
    while (1) {
        // Get random value between 0 and 1000
        randomDelay = esp_random() / (UINT32_MAX/1000);
        ESP_LOGI("task 2", "cnt = %d, wait for %d ms", cnt++, randomDelay);
        vTaskDelay(randomDelay / portTICK_RATE_MS);
    }

    // Delete this task if it exits from the loop above
    vTaskDelete(NULL);
}
