# Tasks

Multitasking is performed on FreeRTOS by creating Tasks or Multiple Tasks. Tasks in FreeRTOS are individual programs which has capability to run independently and synchronously with other tasks. In General, each task is a program of its own with infinite while loop.

```c
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
```

## `xTaskCreate`

Create a new task and add it to the list of tasks that are ready to run.

```c
    xTaskCreate(
        vTaskBlink,  // Function to implement the task
        "Task 1",    // Friendly name of the task
        2048,        // Stack size in bytes, how much memory you want to keep it for this task 
        NULL,        // Task input parameter, NULL for no passing
        5,           // Priority of the task, 1 lowest, 5 highest
        NULL         // Used to pass back a handle
    );
```

## `xTaskCreatePinnedToCore`

Create a new task with a specified affinity and add it to the list of tasks that are ready to run.

```c
   xTaskCreatePinnedToCore(
       vTaskCore,   // Function to implement the task
       "coreTask",  // Friendly name of the task
       1000,        // Stack size in bytes, how much memory you want to keep it for this task
       NULL,        // Task input parameter, NULL for no passing
       2,           // Priority of the task, 1 lowest, 5 highest
       NULL,        // Used to pass back a handle
       coreID       // Core where the task should run
   );
```

## `vTaskDelete`

Remove a task from the RTOS real time kernel's management. The task being deleted will be removed from all ready, blocked, suspended and event lists.

## Naming conventions

* Variables of non stdint types are prefixed `x`. Examples include `BaseType_t` and `TickType_t`.
* API functions are prefixed with their return type, as per the convention defined for variables with the addition of the prefix `v` for void.
* API function names start with the name of the file in which they are defined. For example `vTaskDelete` is defined in `tasks.c`, and has a void return type.

## References

1. https://esp32tutorials.com/esp32-esp-idf-freertos-tutorial-create-tasks/

2. Espressif Systems [Task API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#task-api)

3. https://www.youtube.com/watch?v=k_D_Qu0cgu8

4. https://www.youtube.com/watch?v=ytRRG96cMF0

5. https://github.com/coder137/ESP32-Repo/tree/master/FreeRTOS/Task

6. [Style Guide](https://www.freertos.org/FreeRTOS-Coding-Standard-and-Style-Guide.html)
