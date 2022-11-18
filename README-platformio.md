# Installation

*[PlatformIO](https://platformio.org/) is a cross-platform build system that can be used to develop software platforms for Arduino, ESP32, PIC32, and AVR. PlatformIO can be used with VS Code to provide a powerful set of tools to assist you in development [[1]](https://maker.pro/arduino/tutorial/how-to-use-platformio-in-visual-studio-code-to-program-arduino), [[2]](https://dronebotworkshop.com/platformio/).*

1. Download and install [Visual Studio Code](https://code.visualstudio.com/) source code editor.

2. Run Visual Studio Code application
   1. Open up the extensions viewer in menu **View > Extensions Ctrl+shift+X**.
   2. Type `platformio` into the search bar.
   3. Press **Install** button on the result `PlatformIO IDE`. It may take a few minutes depending on your connection speed. Do not close any window and do not open other folders until the installing process is completed.

      ![Install PlatformIO IDE](images/platformio_install.png)

   4. After that, restart the Visual Studio Code application.

# Usage

1. Create a new project
   1. Click on the PlatformIO icon in the left-hand margin.
   2. In menu **PIO Home** select **Open**.
   3. Click to **New Project** button.
   
      ![Create a new project](images/platformio_new_project.png)
   
   4. In project wizard, type `gpio` as project name
   5. Select ESP32 board you have
   6. Change framework to `Espressif IoT Development Framework` and select location to your working folder
   7. Click on the **Finish** button. The creation of first project may takes a few minutes and internet connection to download needed libraries and frameworks is mandatory.

      ![Create a new project](images/platformio_project_name.png)

2. Code the application
   1. Copy/paste the following example to your `GPIO > src > main.c` file.

```c
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
    // Pin(s) configuration
    gpio_pad_select_gpio(BUILT_IN_LED);
    gpio_set_direction(BUILT_IN_LED, GPIO_MODE_OUTPUT);

    // Standard output to serial monitor
    printf("\nHi, there !\n");
    printf("\033[1;31mBold, red text by ANSI Escape sequences\033[0m\n");

    // Advanced logging functionality for three main levels (Error, Warning, Info)
    ESP_LOGE("setup", "This is Error Logging");
    ESP_LOGW("setup", "This is Warning Logging");
    ESP_LOGI("setup", "This is Info Logging\n");

    // Forever loop
    while (1) {
        gpio_set_level(BUILT_IN_LED, 1);
        ESP_LOGI("gpio", "LED #%d is off", BUILT_IN_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second

        gpio_set_level(BUILT_IN_LED, 0);
        ESP_LOGW("gpio", "LED #%d is on", BUILT_IN_LED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay 1 second
    }
}
```

   2. Add the following settings to `platformio.ini` project configuration file.

```bash
monitor_speed = 115200

# DTR & RTS settings of the serial monitor must be OFF
monitor_rts = 0
monitor_dtr = 0

# Enable ANSI color codes in serial monitor
monitor_raw = yes
```

   3. Connect your ESP32 board to USB port.

   4. Compile and upload the code. The handy shortcuts for building and uploading the code is available on the blue footer taskbar. Hover over the icon to get its functionality. Firts, compile (build) the project and then upload it to ESP32 board. Open the Serial monitor.

      ![Control icons](images/platformio_footer2.png)

## Tested on operating systems

| **Version**                | **Result (yyyy-mm-dd)** | **Note**    |
| :------------------------- | :---------------------: | :---------- |
| Linux Mint 20.3 (Una)      | OK (2022-11-01)         | Laptop      |

```bash
# FYI: How to check OS version in Linux
$ cat /etc/os-release
```
