#ifndef __SHOWTIMETASK_H__
#define __SHOWTIMETASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "u8g2.h"
//#include "beep.h"
#include "Data.h"

void ShowTimeTask(void *params);
void TimerCallBackFun(TimerHandle_t xTimer);

#endif

