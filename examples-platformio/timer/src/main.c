/*
  This example shows how to configure internal timer on ESP32.

  Xtensa dual-core 32-bit LX6 (FireBeetle ESP32), 240 MHz
  PlatformIO, ESP-IDF framework

  Copyright (c) 2023 Tomas Fryza
  Dept. of Radio Electronics, Brno University of Technology, Czechia
  This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.

  See also:
    ESP32 Timers with ESP-IDF
      * https://embeddedexplorer.com/esp32-timer-tutorial/
 */


/*-----------------------------------------------------------*/
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>            // ESP_LOG/E/W/I functions
#include <driver/gpio.h>        // GPIO pins
#include "freertos/semphr.h"
#include "driver/timer.h"


/*-----------------------------------------------------------*/
#define TIMER_DIVIDER   (16)

// On-board LED(s):
// FireBeetle : #2 (blue)
#define LED_PIN 2


/*-----------------------------------------------------------*/
// Tag for ESP_LOG/E/W/I functions
static const char *TAG = "timer";

static SemaphoreHandle_t s_timer_sem;


/*-----------------------------------------------------------*/
static bool IRAM_ATTR timer_group_isr_callback(void * args) {
    BaseType_t high_task_awoken = pdFALSE;
    xSemaphoreGiveFromISR(s_timer_sem, &high_task_awoken);
    return (high_task_awoken == pdTRUE);
}


/*-----------------------------------------------------------*/
void app_main(void)
{
    static int led_state = 0;

    // GPIO
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);

    s_timer_sem = xSemaphoreCreateBinary();
    if (s_timer_sem == NULL) {
        ESP_LOGE(TAG, "binary semaphore can not be created");
    }

    // Timer
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = TIMER_AUTORELOAD_EN
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, (TIMER_BASE_CLK / TIMER_DIVIDER));
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_group_isr_callback, NULL, 0);
    timer_start(TIMER_GROUP_0, TIMER_0);

    // Forever loop
    while (1) {
        if (xSemaphoreTake(s_timer_sem, portMAX_DELAY) == pdPASS) {
            if (led_state == 0) {
                led_state = 1;
                gpio_set_level(LED_PIN, 1);
            } else {
                led_state = 0;
                gpio_set_level(LED_PIN, 0);
            }
        }
    }
}
