/*
   The I2C bus scanner tests all addresses and detects chips and
   modules that are connected to the SDA and SCL signals.
   Xtensa dual-core 32-bit LX6 (ESP32-CAM), 240 MHz
   PlatformIO, ESP-IDF framework

   Copyright (c) 2022 Tomas Fryza
   Dept. of Radio Electronics, Brno University of Technology, Czechia
   This work is licensed under the terms of the GNU GENERAL PUBLIC LICENSE.
 */


/*-----------------------------------------------------------*/
#include <freertos/FreeRTOS.h>  // FreeRTOS
#include <freertos/task.h>
#include <esp_log.h>            // ESP_LOG/E/W/I functions
#include <driver/i2c.h>         // Inter-Integrated Circuit driver


/*-----------------------------------------------------------*/
//        ESP32-CAM
//   * 5V  Arduino     * 3.3V VCC RTC
//   * GND GND RTC     *
//   * 12              * 0    --+ Programming jumper
//   * 13  SDA RTC     * GND  --+
//   * 15  SCL RTC     * VCC
//   * 14              * U0R  Arduino Rx
//   * 2               * U0T  Arduino Tx
//   * 4               * GND  Arduino
//
//        Arduino Uno
//   Reset  --+ Jumper
//   GND    --+
#define I2C_MASTER_SDA_IO 13  // ESP32-CAM
#define I2C_MASTER_SCL_IO 15
#define I2C_MASTER_FREQ_HZ 100000


/*-----------------------------------------------------------*/
// Used function(s)
void vTaskI2CScanner();
void vTaskLoop();


/*-----------------------------------------------------------*/
/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main(void)
{
    ESP_LOGI("setup", "i2c scan application");

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
    ESP_LOGI("i2c", "i2c controller configured");

    // Install i2c driver
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    ESP_LOGI("i2c", "i2c driver installed");

    // Start the i2c scanner task
    xTaskCreate(vTaskI2CScanner, "i2c_scanner", 2048, NULL, 5, NULL);
}


/*-----------------------------------------------------------*/
void vTaskI2CScanner()
{
    uint8_t devices_found = 0;

    ESP_LOGI("i2c", "scanning the bus...");
    for (uint8_t sla = 8; sla < 119; sla++) {
        // Create the i2c commands list
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();

        // Queue a "START" signal to a list
        i2c_master_start(cmd);

        // Slave address to write data
        i2c_master_write_byte(cmd, (sla<<1) | I2C_MASTER_WRITE, true);

        // Queue a "STOP" signal to a list
        i2c_master_stop(cmd);

        ESP_LOGI("i2c", "0x%02x", sla);

        // Send all the queued commands on the I2C bus, in master mode
        if (i2c_master_cmd_begin(I2C_NUM_0, cmd, (1000/portTICK_RATE_MS)) == ESP_OK) {
            ESP_LOGI("i2c", "found device with address 0x%02x", sla);
            devices_found++;
        }

        // Free the I2C commands list
        i2c_cmd_link_delete(cmd);

        vTaskDelay(25 / portTICK_PERIOD_MS);  // 25 milliseconds
    }
    ESP_LOGI("i2c", "...scan completed!");
    ESP_LOGI("i2c", "#%d device(s) found", devices_found);

    // Start the loop task
    xTaskCreate(vTaskLoop, "forever_loop", 2048, NULL, 5, NULL);

    // Delete this task
    vTaskDelete(NULL);
}


/*-----------------------------------------------------------*/
void vTaskLoop()
{
    // Forever loop
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // Delete this task if it exits from the loop above
    vTaskDelete(NULL);
}
