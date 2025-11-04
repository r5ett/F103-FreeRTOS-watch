#ifndef __SHOWCLOCK_H__
#define __SHOWCLOCK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "queue.h"
//#include "beep.h"
#include "u8g2.h"
#include "Data.h"
#include "stdio.h"
#include "math.h"

void ShowClockTimeTask(void *params);
void ClockTimerCallBackFun(TimerHandle_t xTimer);

#endif

