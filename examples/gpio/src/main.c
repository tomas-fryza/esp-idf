#include <stdio.h>              // Needed for "printf" function
#include <esp_log.h>            // ESP_LOGE/W/I functions
#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>      // vTaskDelay, portTICK_PERIOD_MS
#include <driver/gpio.h>        // GPIO pins

// ESP32-CAM on-board LEDs: #33 (red, bottom side), #4 (Flash, top side)
#define BUILT_IN_LED 33

// In ESP-IDF instead of "main", we use "app_main" function
// where the program execution begins
void app_main()
{
    // Pin(s) configuration:
    gpio_pad_select_gpio(BUILT_IN_LED);
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);

    // Standard output to serial monitor
    printf("\nHi, there !\n");
    printf("\033[1;31mBold, red text by ANSI Escape sequences\033[0m\n");

    // Advanced logging functionality for three main levels (Error, Warning, Info)
    uint16_t log_level = 1;
    ESP_LOGE("setup", "This is Error Logging with level %d", log_level++);
    ESP_LOGW("setup", "This is Warning Logging with level %d", log_level++);
    ESP_LOGI("setup", "This is Info Logging with level %d\n", log_level++);

    while (1) {
        gpio_set_level(BUILT_IN_LED, 1);
        ESP_LOGI("gpio", "LED #%d is off", BUILT_IN_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second

        gpio_set_level(BUILT_IN_LED, 0);
        ESP_LOGW("gpio", "LED #%d is on", BUILT_IN_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second
    }
}
