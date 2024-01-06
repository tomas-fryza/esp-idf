/*
   Read temperature and humidity from DHT12 sensor, SLA = 0x5c.

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
//   * 5V  Arduino     * 3.3V VCC DHT
//   * GND GND DHT     *
//   * 12              * 0    --+ Programming jumper
//   * 13  SDA DHT     * GND  --+
//   * 15  SCL DHT     * VCC
//   * 14              * U0R  Arduino Rx
//   * 2               * U0T  Arduino Tx
//   * 4               * GND  Arduino
//
//        Arduino Uno
//   Reset  --+ Jumper
//   GND    --+
//
// ESP32-CAM: SDA #13, SCL #15
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_ACK 0x0
#define I2C_NACK 0x1
#define I2C_DHT_ADDRESS 0x5c  // DHT12 temp & humidity sensor
#define I2C_DHT_HUMID 0x00    // 0x00 @ Humidity
#define I2C_DHT_TEMP 0x02     // 0x02 @ Temperature


/*-----------------------------------------------------------*/
// Used function(s)
void dht_sensor_task();
void dht_get_temp();
void dht_get_humid();
void dht_get_all_values();


/*-----------------------------------------------------------*/
// Declaration of "dht12" variable with structure "DHT_values_structure"
struct DHT_values_structure {
    uint8_t humidInt;
    uint8_t humidDec;
    uint8_t tempInt;
    uint8_t tempDec;
    uint8_t checksum;
} dht12;


/*-----------------------------------------------------------*/
/* In ESP-IDF instead of "main", we use "app_main" function
   where the program execution begins */
void app_main(void)
{
    ESP_LOGI("setup", "i2c sensor application");

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

    // Start I2C sensor task
    xTaskCreate(dht_sensor_task, "read_sensor_values", 2048, NULL, 5, NULL);
}


/*-----------------------------------------------------------*/
void dht_sensor_task()
{
    ESP_LOGI("task", "DHT sensor task started");

    // Forever loop
    while (1) {
        // dht_get_temp();
        // vTaskDelay(20 / portTICK_PERIOD_MS);
        // dht_get_humid();
        // vTaskDelay(20 / portTICK_PERIOD_MS);
        dht_get_all_values();
        ESP_LOGI("i2c", "temperature: %d.%d °C", dht12.tempInt, dht12.tempDec);
        ESP_LOGI("i2c", "humidity: %d.%d", dht12.humidInt, dht12.humidDec);
        ESP_LOGI("i2c", "checksum: %d", dht12.checksum);

        // Delay 5 seconds
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    // Delete this task if it exits from the loop above
    vTaskDelete(NULL);
}


/*-----------------------------------------------------------*/
void dht_get_temp()
{
    // Create and execute i2c commands list
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, I2C_DHT_TEMP, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &dht12.tempInt, I2C_ACK);
    i2c_master_read_byte(cmd, &dht12.tempDec, I2C_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, (1000 / portTICK_RATE_MS));
    i2c_cmd_link_delete(cmd);
}


/*-----------------------------------------------------------*/
void dht_get_humid()
{
    // Create and execute i2c commands list
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, I2C_DHT_HUMID, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_DHT_ADDRESS<<1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &dht12.humidInt, I2C_ACK);
    i2c_master_read_byte(cmd, &dht12.humidDec, I2C_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, (1000 / portTICK_RATE_MS));
    i2c_cmd_link_delete(cmd);
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
