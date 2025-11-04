#ifndef __SHOWMENU_H__
#define __SHOWMENU_H__

#include "stm32f1xx_hal.h"
#include "u8g2.h"
#include "Data.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "driver_passive_buzzer.h"
#include "gpio.h"
#include "task.h"

void ShowUI(void);
void ShowMenuTask(void *params);

#endif
