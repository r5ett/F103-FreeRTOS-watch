#ifndef __SHOWFLASHLIGHT_H__
#define __SHOWFLASHLIGHT_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "queue.h"
//#include "beep.h"
#include "u8g2.h"
#include "Data.h"

void ShowFlashLightTask(void *params);

#endif

