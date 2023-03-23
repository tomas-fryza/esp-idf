# ESP-IDF examples

ESP-IDF is Espressif's official [IoT Development Framework](https://www.espressif.com/en/products/sdks/esp-idf) for the ESP32, ESP32-S, ESP32-C and ESP32-H series of SoCs. It provides a self-sufficient Software Development Kit (SDK) for any generic application development on these platforms, using programming languages such as C and C++.

## ESP-IDF VS Code extension

* [How to install and use ESP32 ESP-IDF in VS Code for Windows](https://esp32tutorials.com/install-esp32-esp-idf-windows-integrate-vs-code/)
* [ ] How to configure ESP32 ESP-IDF in Linux

### Useful commands in VS Code
Command | Description
:-- | :--
`F1` | Command Palette where all commands are found
`Ctrl+E T` | Open ESP-IDF Terminal where you can use `idf.py` command-line tool
`idf.py create-project NAME` | Create a new project titles NAME
`idf.py build` | Build the project
`idf.py flash monitor` | Flashing esp-idf project and open serial monitor
`Ctrl+]`, `Ctrl+)` | Exit the monitor

## Use with PlatformIO

* [How to install and use ESP32 ESP-IDF in PlatformIO](docs/README-platformio.md)

### Examples

* [x] [Basic template with GPIO](examples/gpio)
* [x] [Logging methods](examples/log_methods)
* [x] [Tasks](examples/tasks)
* [x] [I2C scan](examples/i2c_scan)
* [x] [I2C DHT12 sensor](examples/i2c_sensor)
* [x] [Wi-Fi APs scan](examples/wifi_scan)
* [x] [Wi-Fi GET requests to HTTP](examples/wifi_get_requests)
* [x] [Wi-Fi & DHT12 & ThingSpeak](examples/wifi_thingspeak)
* [ ] Timer(s), PWM
* [ ] SPI
* [ ] Bluetooth
* [ ] Wi-Fi GET requests to HTTPs
* [ ] Wi-Fi & AWS
* [ ] ADC, DAC
* [ ] CAN
* [ ] IR
* [ ] SHA

## General information

* [ESP32 brief overview](https://www.youtube.com/watch?v=DoctWoxIaH8) (YouTube video)
* [Git commands](docs/README-useful-git-commands.md)
* [ ] ESP32 structure, versions
* [ ] Pinout of ESP32-CAM board
* [ ] Pinout of FireBeetle board
* [ ] How to program ESP32-CAM board via Arduino Uno

## References

1. Espressif Systems. [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)

2. [ESP32 for IoT: A Complete Guide](https://www.nabto.com/guide-to-iot-esp-32/)

3. [How To Choose The Best RTOS for IoT Devices](https://www.nabto.com/how-to-choose-best-rtos-for-iot/)
