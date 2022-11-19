#include <stdio.h>              // Needed for "printf" function
#include <esp_log.h>            // ESP_LOG/E/W/I functions
#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>      // vTaskDelay, portTICK_PERIOD_MS
#include <driver/gpio.h>        // GPIO pins

// ESP32-CAM on-board LEDs: #33 (red, bottom side), #4 (Flash, top side)
#define BUILT_IN_LED 33

/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main()
{
    // Standard output to serial monitor
    printf("\nHi, there!\n");

    // Advanced logging functionality for three main levels (Error, Warning, Info)
    ESP_LOGE("setup", "this is Error Logging");
    ESP_LOGW("setup", "this is Warning Logging");
    ESP_LOGI("setup", "this is Info Logging");

    // Pin(s) configuration
    gpio_pad_select_gpio(BUILT_IN_LED);
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);
    ESP_LOGI("gpio", "pin #%d configured as output", BUILT_IN_LED);

    // Forever loop
    while (1)
    {
        gpio_set_level(BUILT_IN_LED, 1);        // Set high level
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second

        gpio_set_level(BUILT_IN_LED, 0);        // Set low level
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second
    }

    // Will never reach this
}
