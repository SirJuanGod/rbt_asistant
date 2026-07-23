#ifndef CONTROL_TASK_H
#define CONTROL_TASK_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "core/system_types.h"
#include "drivers/neopixel_driver.h"
#include "drivers/ir_driver.h"

bool startControlTask(NeoPixelDriver* neopixelDriver, IrDriver* irDriver);
NovaState getCurrentNovaState();

#endif // CONTROL_TASK_H
