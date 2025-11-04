/* USER CODE BEGIN Header */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_mpu6050.h"
#include "driver_timer.h"
#include "driver_ds18b20.h"
#include "driver_dht11.h"
#include "driver_active_buzzer.h"
#include "driver_passive_buzzer.h"
#include "driver_color_led.h"
#include "driver_ir_receiver.h"
#include "driver_ir_sender.h"
#include "driver_light_sensor.h"
#include "driver_ir_obstacle.h"
#include "driver_ultrasonic_sr04.h"
#include "driver_spiflash_w25q64.h"
#include "driver_rotary_encoder.h"
#include "driver_motor.h"
#include "driver_key.h"
#include "driver_uart.h"

/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "event_groups.h"
#include "queue.h"
#include "u8g2.h"
//#include "beep.h"
#include "Data.h"
#include "ShowTimeTask.h"
#include "ShowMenu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define Task_default_size 128

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
// 定时器句柄
TimerHandle_t g_Timer;
TimerHandle_t g_Clock_Timer;
// 任务句柄
TaskHandle_t xShowTimeTaskHandle = NULL;
TaskHandle_t xShowMenuTaskHandle = NULL;
TaskHandle_t xShowCalendarTaskHandle = NULL;
TaskHandle_t xShowClockTaskHandle = NULL;
TaskHandle_t xShowFlashLightTaskHandle = NULL;
TaskHandle_t xShowSettingTaskHandle = NULL;
TaskHandle_t xShowWoodenFishTaskHandle = NULL;
TaskHandle_t xShowDHT11TaskHandle = NULL;

// 队列
QueueHandle_t g_xQueueMenu; 

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
// 任务函数
extern void ClockTimerCallBackFun(void);
extern void ShowDHT11Task(void *params);
extern void ShowCalendarTask(void *params);
extern void ShowFlashLightTask(void *params);
extern void ShowWoodenFishTask(void *params);
extern void ShowClockTimeTask(void *params);
extern void ShowSetting_Task(void *params);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
	// 创建定时器TimerHandle_t xTimerCreate(定时器名字， 周期（ms）， 类型（pdTRUE表示自动加载, pdFALSE表示一次性）， 唯一标识ID， 定时器到期时触发的回调函数)
	g_Timer = xTimerCreate("Timer1", 1000, pdTRUE, NULL, (TimerCallbackFunction_t)TimerCallBackFun);
	g_Clock_Timer = xTimerCreate("Timer2", 100, pdTRUE, NULL, (TimerCallbackFunction_t)ClockTimerCallBackFun);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
	
	
  /* Create the thread(s) */
  /* creation of defaultTask */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  // 创建任务BaseType_t xTaskCreate(任务函数， 任务的名字， 栈大小， 调用任务函数时传入的参数， 优先级， 任务句柄)
	xTaskCreate(ShowTimeTask, "ShowTimeTask", 128, NULL, osPriorityNormal, &xShowTimeTaskHandle);
	xTaskCreate(ShowMenuTask, "ShowMenuTask", 256, NULL, osPriorityNormal, &xShowMenuTaskHandle);		
	//5个APP
  xTaskCreate(ShowCalendarTask, "ShowCalendarTask", 256, NULL, osPriorityNormal, &xShowCalendarTaskHandle);
  xTaskCreate(ShowFlashLightTask, "ShowFlashLightTask", Task_default_size, NULL, osPriorityNormal, &xShowFlashLightTaskHandle);
  xTaskCreate(ShowDHT11Task, "ShowDHT11Task", Task_default_size, NULL, osPriorityNormal, &xShowDHT11TaskHandle);
	//xTaskCreate(ShowWoodenFishTask, "ShowWoodenFishTask", Task_default_size, NULL, osPriorityNormal, &xShowWoodenFishTaskHandle);
  xTaskCreate(ShowClockTimeTask, "ShowClockTimeTask", Task_default_size, NULL, osPriorityNormal, &xShowClockTaskHandle);
  xTaskCreate(ShowSetting_Task, "ShowSetting_Task", 256, NULL, osPriorityNormal, &xShowSettingTaskHandle);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
// 默认任务
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  if(g_Timer != NULL)// 检查定时器句柄g_Timer是否有效
	{
		// 启动定时器g_Timer，第二个参数0表示“启动定时器的超时时间为0”（即立即启动，不等待）
    // 启动后定时器开始计时，按之前配置的1秒周期触发回调函数TimerCallBackFun
		xTimerStart(g_Timer, 0);
	}
	for(;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/****
		* @brief  GPIO外部中断回调函数：当指定GPIO引脚产生外部中断时，自动调用此函数处理中断
		* @param  uint16_t GPIO_Pin：产生中断的GPIO引脚编号
		* @retval 无
*****/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{	
	// 按键中断：将按键数据发送到消息队列	
	// 部分数据可能暂未使用
	extern BaseType_t end_flag;
	extern BaseType_t seclect_end;
	BaseType_t  RM_Flag, LM_Flag, EN_Flag, EX_Flag;// 按键标志位：分别代表右键、左键、确认键、退出键
	Key_data key_data;
	// 若中断来自GPIO_PIN_11
  if(GPIO_Pin == GPIO_PIN_11)
	{ 
		for(int i = 0; i<5000; i++){}// 简单软件延时（约几毫秒）：用于按键消抖
		if(end_flag == 1&&seclect_end == 0)// 检查状态：只有当end_flag=1且seclect_end=0时，才处理此按键
		{
			RM_Flag = 1;// 标记“右键”被按下
			key_data.rdata = RM_Flag;// 将右键标志存入按键数据结构体的rdata成员
			// 从中断服务程序（ISR）中向队列g_xQueueMenu尾部发送按键数据（中断安全的队列发送函数）
			// 最后一个参数为NULL：表示不等待队列空间，发送失败则忽略（中断中不能阻塞）
			xQueueSendToBackFromISR(g_xQueueMenu, &key_data, NULL);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);// 拉低GPIOC_PIN13（可能用于指示按键被处理）
			RM_Flag = 0;// 重置右键标志（避免重复触发）						
		}
	}
	if(GPIO_Pin == GPIO_PIN_10)// 若中断来自GPIO_PIN_10（对应“左键”）
	{ 
		for(int i = 0; i<5000; i++){}
		if(end_flag == 1&&seclect_end == 0)
		{
		 	LM_Flag = 1;
			key_data.ldata = LM_Flag;
			xQueueSendToBackFromISR(g_xQueueMenu, &key_data, NULL);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
			LM_Flag = 0;
		}
	}
	if(GPIO_Pin == GPIO_PIN_1)// 若中断来自GPIO_PIN_1（对应“确认键”）
	{
		for(int i = 0; i<5000; i++){}		
		if(end_flag == 1&&seclect_end == 0)
		{
			EN_Flag = 1;
			key_data.updata = EN_Flag;
			xQueueSendToBackFromISR(g_xQueueMenu, &key_data, NULL);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
			EN_Flag = 0;
		}
	}
	if(GPIO_Pin == GPIO_PIN_0)// 若中断来自GPIO_PIN_0（对应“退出键”）
	{
		for(int i = 0; i<5000; i++){}		
		if(end_flag == 1&&seclect_end == 0)
		{
			EX_Flag = 1;
			key_data.exdata = EX_Flag;
			if(end_flag == 1&&seclect_end == 0)xQueueSendToBackFromISR(g_xQueueMenu, &key_data, NULL);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
			EX_Flag = 0;
		}
	}
}

/* USER CODE END Application */
