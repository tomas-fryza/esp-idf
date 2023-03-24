# ESP-IDF examples

ESP-IDF is Espressif's official [IoT Development Framework](https://www.espressif.com/en/products/sdks/esp-idf) for the ESP32, ESP32-S, ESP32-C and ESP32-H series of SoCs. It provides a self-sufficient Software Development Kit (SDK) for any generic application development on these platforms, using programming languages such as C and C++.

## Use with VS Code

* [How to integrate ESP-IDF with VS Code for Windows](https://esp32tutorials.com/install-esp32-esp-idf-windows-integrate-vs-code/)
* [ ] How to configure ESP-IDF in Linux

### Useful commands in VS Code

Command | Description
:-- | :--
`F1` | Command Palette where all commands are found
`F1` `ESP-IDF: New project` | Create a new project via Command Palette
`F1` `ESP-IDF: Open ESP-IDF Terminal` | Open ESP-IDF Terminal where you can use `idf.py` command-line tool
`Ctrl+E T` | Another way how to open ESP-IDF Terminal
`idf.py create-project NAME` | Create a new project titled `NAME`
`idf.py menuconfig` | Open project configuration
`idf.py build` | Build the project
`idf.py flash monitor` | Flashing esp-idf project and open serial monitor
`Ctrl+]`, `Ctrl+)` | Exit the monitor

## Use with PlatformIO

* [How to install and use ESP32 ESP-IDF in PlatformIO](docs/README-platformio.md)

### PlatformIO examples

* [Basic template with GPIO](examples/gpio)
* [Logging methods](examples/log_methods)
* [Tasks](examples/tasks)
* [I2C scan](examples/i2c_scan)
* [I2C DHT12 sensor](examples/i2c_sensor)
* [Wi-Fi APs scan](examples/wifi_scan)
* [Wi-Fi GET requests to HTTP](examples/wifi_get_requests)
* [Wi-Fi & DHT12 & ThingSpeak](examples/wifi_thingspeak)

**TODO:**

* [ ] Timer(s), PWM
* [ ] SPI
* [ ] Bluetooth
* [ ] Wi-Fi GET requests to HTTPs
* [ ] Wi-Fi & AWS
* [ ] ADC, DAC
* [ ] CAN
* [ ] IR
* [ ] SHA

## Useful information

* [ESP32 brief overview](https://www.youtube.com/watch?v=DoctWoxIaH8) (YouTube video)
* [Git commands](docs/README-useful-git-commands.md)
* [ESP32-CAM board](docs/esp32-cam.md)
* [FireBeetle board](docs/firebeetle.md)
* [ ] ESP32 structure, versions

## References

1. Espressif Systems. [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)

2. Esp32 Tutorials. [Install ESP32 ESP-IDF on Windows and Integrate with VS code](https://esp32tutorials.com/install-esp32-esp-idf-windows-integrate-vs-code/)

3. [ESP32 for IoT: A Complete Guide](https://www.nabto.com/guide-to-iot-esp-32/)

4. Random Nerd Tutorials. [ESP32-CAM AI-Thinker Pinout Guide: GPIOs Usage Explained](https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/)

5. Renzo Mischianti. [ESP32-CAM: pinout, specs and Arduino IDE configuration – 1](https://www.mischianti.org/2021/08/30/esp32-cam-pinout-specs-and-arduino-ide-configuration-1/)

6. [How To Choose The Best RTOS for IoT Devices](https://www.nabto.com/how-to-choose-best-rtos-for-iot/)
