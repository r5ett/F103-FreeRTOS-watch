#ifndef __SHOWDHT11_H__
#define __SHOWDHT11_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "queue.h"
#include <stdio.h>
//#include "beep.h"
#include "driver_dht11.h"
#include "u8g2.h"
#include "Data.h"
#include "ShowCalendar.h"

void ShowDHT11Task(void *params);

#endif

