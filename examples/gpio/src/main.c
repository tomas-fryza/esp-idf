/*
   Blink a LED.
   Xtensa dual-core 32-bit LX6 (ESP32-CAM), 240 MHz
   PlatformIO, ESP-IDF framework

   Copyright (c) 2022 Tomas Fryza
   Dept. of Radio Electronics, Brno University of Technology, Czechia
   This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.
 */


/*-----------------------------------------------------------*/
#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>      // vTaskDelay, portTICK_PERIOD_MS
#include <driver/gpio.h>        // GPIO pins


/*-----------------------------------------------------------*/
// On-board LED(s):
// FireBeetle : #2 (blue)
// ESP32-CAM  : #33 (red, bottom side), #4 (Flash, top side)
#define BUILT_IN_LED 2


/*-----------------------------------------------------------*/
/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main()
{
    /* Reset an gpio to default state (enable pullup, disable
       interrupts, ...) */
    gpio_reset_pin(BUILT_IN_LED);
    // Set output
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);

    // Forever loop
    while (1) {
        gpio_set_level(BUILT_IN_LED, 1);        // Set high level
        vTaskDelay(100 / portTICK_PERIOD_MS);   // Delay 100 milliseconds

        gpio_set_level(BUILT_IN_LED, 0);        // Set low level
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second
    }

    // Will never reach this
}
