#ifndef __SHOWWOODENFISH_H__
#define __SHOWWOODENFISH_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
//#include "beep.h"
#include "u8g2.h"
#include "Data.h"

void ShowWoodenFishTask(void *params);

#endif
