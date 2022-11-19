# Basic ESP-IDF template

## `vTaskDelay`

Delay a task for a given number of ticks. `vTaskDelay()` specifies a time at which the task wishes to unblock relative to the time at which `vTaskDelay()` is called. For example, specifying a block period of 100 ticks will cause the task to unblock 100 ticks after `vTaskDelay()` is called.

```c
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

...
vTaskDelay(1000 / portTICK_PERIOD_MS);
```

## References

https://www.youtube.com/watch?v=r3yziWNAfe0

1. Espressif Systems [Task API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#task-api)
