#include "ShowTimeTask.h"

/*----------------------------------外部变量----------------------------------*/
extern QueueHandle_t g_xQueueMenu;
extern TaskHandle_t xShowMenuTaskHandle;
extern TaskHandle_t xShowTimeTaskHandle;
extern TaskHandle_t xShowWoodenFishTaskHandle;
extern TaskHandle_t xShowFlashLightTaskHandle;
extern TaskHandle_t xShowSettingTaskHandle;
extern TaskHandle_t xShowCalendarTaskHandle;
extern TaskHandle_t xShowClockTaskHandle;
extern TaskHandle_t xShowDHT11TaskHandle;

#define BOX_R 1// 框/边框的圆角半径（或线宽），值为1像素
uint8_t time_flag = 0;// 时间状态标志位

uint8_t sec_unit;// 秒的个位数字（0-9）：存储当前秒数的个位部分（如“56秒”中的“6”）
uint8_t sec_decade;// 秒的十位数字（0-5）：存储当前秒数的十位部分（如“56秒”中的“5”）
uint8_t min_unit;// 分的个位数字（0-9）：存储当前分钟的个位部分（如“23分”中的“3”）
uint8_t min_decade;// 分的十位数字（0-5）：存储当前分钟的十位部分（如“23分”中的“2”）
uint8_t hour_unit;// 时的个位数字（0-9或0-3，取决于12/24小时制）：存储当前小时的个位部分（如“15时”中的“5”）
uint8_t hour_decade;// 时的十位数字（0-2，24小时制下）：存储当前小时的十位部分（如“15时”中的“1”）

// // 定义时间相关元素的绘制参数结构体
typedef struct Time_param{
  int x[4];// 4个元素的X坐标数组：通常对应4位时间数字（如时高位、时低位、分高位、分低位）的水平位置（屏幕X轴坐标）
	int y;// 垂直位置（屏幕Y轴坐标）：4位时间数字的共同Y坐标（同一行显示时，垂直位置相同）
	int w;// 宽度（width）：单个时间数字的宽度（如6像素，对应之前的num_w）
	int h;// 高度（height）：单个时间数字的高度（如8像素，对应之前的num_h）
	int x_arg;// X方向偏移量/间隔
}T;

T time = { {8, 35, 71, 98}, 15, 20, 40, 98};
Image Box1 = {62, 22, 4, 4,};
Image Box2 = {62, 39, 4, 4,};

/****
		* @brief  显示时间任务
		* @param  params：任务创建时传递的参数
		* @retval 无
*****/
void ShowTimeTask(void *params)
{
	//buzzer_init();
	//xSemaphoreTake(g_xSemTicks, portMAX_DELAY);

	// 挂起其他任务（确保当前时间界面独占运行）
	vTaskSuspend(xShowMenuTaskHandle);
	//vTaskSuspend(xShowWoodenFishTaskHandle);
	vTaskSuspend(xShowFlashLightTaskHandle);
	vTaskSuspend(xShowSettingTaskHandle);
  vTaskSuspend(xShowClockTaskHandle);
  vTaskSuspend(xShowCalendarTaskHandle);
  vTaskSuspend(xShowDHT11TaskHandle);

	// 创建消息队列（接收按键消息）
	g_xQueueMenu = xQueueCreate(1, 4);
	
	u8g2_t u8g2;
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0, u8x8_byte_hw_i2c, u8g2_stm32_delay);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_ClearDisplay(&u8g2);
	//u8g2_SetFont(&u8g2, u8g2_font_wqy16_t_chinese1);
	u8g2_SetFont(&u8g2, u8g2_font_fur35_tf);
	
	struct Key_data	key_data;

	while(1)
	{	
		u8g2_ClearBuffer(&u8g2);
		
		// 绘制状态图标
		u8g2_DrawXBMP(&u8g2, 0, 0, 23, 10, ShowPower);
		u8g2_DrawXBMP(&u8g2, 105, 0, 23, 10, ShowGame);
		// 绘制时间数字及分隔元素
		u8g2_DrawXBMP(&u8g2, time.x[3], time.y, time.w, time.h, BigNum[sec_unit]);// 绘制秒的个位数字：使用time结构体定义的位置（x[3], y）、尺寸（w, h），从BigNum数组取对应数字的位图
		u8g2_DrawXBMP(&u8g2, time.x[2], time.y, time.w, time.h, BigNum[sec_decade]);// 绘制秒的十位数字：位置x[2]
		u8g2_DrawRBox(&u8g2, Box1.x, Box1.y, Box1.w, Box1.h, BOX_R);// 绘制第一个圆角矩形 冒号的上半部分
		u8g2_DrawRBox(&u8g2, Box2.x, Box2.y, Box2.w, Box2.h, BOX_R);// 绘制第二个圆角矩形	冒号的下半部分
		u8g2_DrawXBMP(&u8g2, time.x[1], time.y, time.w, time.h, BigNum[min_unit]);// 绘制分钟的个位数字
		u8g2_DrawXBMP(&u8g2, time.x[0], time.y, time.w, time.h, BigNum[min_decade]);// 绘制分钟的十位数字
		u8g2_DrawXBMP(&u8g2, 56, 2, 6, 8, Num_6x8[hour_decade]);// 绘制小时的十位数字
		u8g2_DrawXBMP(&u8g2, 66, 2, 6, 8, Num_6x8[hour_unit]);// 绘制小时的个位数字
		u8g2_SendBuffer(&u8g2);

		vTaskDelay(250);
		// 处理按键消息队列
		if(time_flag == 0)
		{
			// 从队列接收按键数据
			pdPASS == xQueueReceive(g_xQueueMenu, &key_data, 0);
		}
		// 任务调度：检测到上键时返回主菜单
		if(key_data.updata == 1)// 上键被按下
		{	
			//buzzer_buzz(2500, 100);			                     
			vTaskResume(xShowMenuTaskHandle);// 恢复主菜单任务
			vTaskSuspend(NULL);// 挂起当前时间任务
			key_data.updata = 0;// 清除按键状态
		}			
	}
}

/****
		* @brief  定时更新秒、分、时的数值（处理各数位的进位）
		* @param  TimerHandle_t xTimer：触发该回调的定时器句柄
		* @retval 无
*****/
void TimerCallBackFun(TimerHandle_t xTimer)
{  
	// 处理时间数据：更新秒、分、时的各数位并处理进位
	sec_unit++;// 秒的个位数字递增	
	if(sec_unit>9){sec_unit = 0; sec_decade++;}// 若秒的个位超过9（如9→10），则秒个位重置为0，秒的十位数字递增
	if(sec_decade>5){sec_decade = 0; min_unit++;}// 若秒的十位超过5（秒的最大值为59，十位最大为5），则秒十位重置为0，分的个位数字递增
	if(min_unit>9){min_unit = 0; min_decade++;}// 若分的个位超过9（如9→10），则分个位重置为0，分的十位数字递增
	if(min_decade>5){min_decade = 0; hour_unit++;}// 若分的十位超过5（分的最大值为59，十位最大为5），则分十位重置为0，时的个位数字递增
	if(hour_unit>5){hour_unit = 0; hour_decade++;}// 若时的个位超过5（根据场景可能是12/24小时制的简化处理，如“15时”的个位5+1后需进位），则时个位重置为0，时的十位数字递增
}
