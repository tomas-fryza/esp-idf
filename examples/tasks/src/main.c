/*
 * Task(s) implementation
 */

#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>      // vTaskDelay, portTICK_PERIOD_MS
#include <esp_log.h>            // ESP_LOGE/W/I functions
#include <driver/gpio.h>        // GPIO pins

// ESP32-CAM on-board LEDs: #33 (red, bottom side), #4 (Flash, top side)
#define BUILT_IN_LED 33

// Used function(s)
void blink_task(void *taskParams);

// In ESP-IDF instead of "main", we use "app_main" function
// where the program execution begins
void app_main()
{
    ESP_LOGI("main", "Application started");
    //                                 Stack depth  Priority
    xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);
    xTaskCreate(&blink_task, "blink_task", 2048, NULL, 3, NULL);
}
/*-----------------------------------------------------------*/

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
	// Init the GPIO as output
    gpio_pad_select_gpio(BUILT_IN_LED);
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);

    // Forever loop
    while (1)
    {
        gpio_set_level(BUILT_IN_LED, xPortGetCoreID());
        ESP_LOGI("gpio", "core #%d", xPortGetCoreID());
        vTaskDelay(333*(xPortGetCoreID()+1) / portTICK_PERIOD_MS);  // Delay 1 second

//        gpio_set_level(BUILT_IN_LED, 0);
//        ESP_LOGW("gpio", "core #%d, LED #%d is on", xPortGetCoreID(), BUILT_IN_LED);
//        vTaskDelay(333*(xPortGetCoreID()+1) / portTICK_PERIOD_MS);  // Delay 1 second
    }

    vTaskDelete(NULL);  // Delete this task if it exits from the loop above
}
/*-----------------------------------------------------------*/
