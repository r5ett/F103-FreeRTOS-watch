#include "ShowClock.h"

/*----------------------------------外部变量----------------------------------*/
extern QueueHandle_t g_xQueueMenu;
extern TaskHandle_t xShowMenuTaskHandle;
extern TimerHandle_t g_Clock_Timer;
extern u8g2_t u8g2;

uint16_t millisecond;// 毫秒计数器
uint8_t len1, len2;// 像素长度
uint8_t clock_flag = 0;// 时钟状态标志位
int8_t seclect_flag = 0;// 时间设置选中项标志
uint16_t g_num_x[] = {1, 9, 25, 33, 41, 17};// X坐标数组
uint16_t g_num_y[] = {22, 52};// Y坐标数组
uint16_t num_w = 6;// 宽
uint16_t num_h = 8;// 高
uint16_t g_seclect_x[] = {1, 9, 25, 33, 41};// 选中框 X 坐标数组
uint16_t g_clock_num[] = {0, 0, 0, 0};// 时钟显示数字缓存
uint16_t g_real_time[] = {0, 0, 0, 0};// 实时时间数据缓存

/****
		* @brief  时钟UI绘制函数：绘制时间设置区、实时时间区、圆形时钟图案及选中框
		* @param  无
		* @retval 无
*****/
void ShowClock(void)
{
	// 绘制"Set:"字符串（设置时间区域的标题），并获取其长度（像素）存入len1
  // 显示位置：X=0，Y=30（屏幕上半部分，作为设置时间的标题）
	len1 = u8g2_DrawStr(&u8g2, 0, 30, "Set:");
	// 绘制"Ret:"字符串（实时时间区域的标题），并获取其长度（像素）存入len2
  // 显示位置：X=0，Y=60（屏幕下半部分，作为实时时间的标题）
	len2 = u8g2_DrawStr(&u8g2, 0, 60, "Ret:");

	// 绘制时间数字（设置时间和实时时间）
	// 循环4次（对应时间的4位数字：时高位、时低位、分高位、分低位）
	for(int i = 0; i < 4; i++)
	{
		u8g2_DrawXBMP(&u8g2, len1+g_num_x[i], g_num_y[0], num_w, num_h, Num_6x8[g_clock_num[i]]);	
		u8g2_DrawXBMP(&u8g2, len2+g_num_x[i], g_num_y[1], num_w, num_h, Num_6x8[g_real_time[i]]);
	}
	u8g2_DrawXBMP(&u8g2, len1+g_num_x[5], g_num_y[0], num_w, num_h, Num_6x8[10]);/* : */
	u8g2_DrawXBMP(&u8g2, len1+g_num_x[4], g_num_y[0], num_w, num_h, Num_6x8[11]);/* > */
	u8g2_DrawXBMP(&u8g2, len2+g_num_x[5], g_num_y[1], num_w, num_h, Num_6x8[10]);/* : */

	//绘制圆形时钟图案（模拟表盘）
	u8g2_DrawCircle(&u8g2, 104, 31, 22, U8G2_DRAW_ALL);// 绘制内圆：圆心(104,31)，半径22，绘制完整圆形（U8G2_DRAW_ALL）
	u8g2_DrawCircle(&u8g2, 104, 31, 23, U8G2_DRAW_ALL);// 绘制外圆：圆心(104,31)，半径23（比内圆大1像素，形成边框效果）
	u8g2_DrawDisc(&u8g2, 104, 31, 1, U8G2_DRAW_ALL);// 绘制圆心点：圆心(104,31)，半径1的实心圆（表盘中心）
	// 绘制圆形时钟内的数字
	u8g2_DrawXBMP(&u8g2, 94, 12, 20, 40, BigNum[millisecond]);			
	// 绘制当前选中项的高亮框
	u8g2_DrawFrame(&u8g2, len1+g_seclect_x[seclect_flag]-1, g_num_y[0]-2, num_w+2, num_h+3);	
}

/****
		* @brief  控制时钟定时功能的UI刷新、按键交互及定时逻辑
		* @param  params：任务创建时传递的参数
		* @retval 无
*****/
void ShowClockTimeTask(void *params)
{
	//buzzer_init();

	/* 创建队列 */
	g_xQueueMenu = xQueueCreate(1, 4);
	// 若队列创建成功，点亮GPIOC引脚13
	if(NULL != g_xQueueMenu)HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

	// 初始化u8g2显示配置
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0, u8x8_byte_hw_i2c, u8g2_stm32_delay);
	u8g2_InitDisplay(&u8g2); 
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_ClearDisplay(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_wqy16_t_chinese1);
	//u8g2_SetFont(&u8g2, u8g2_font_spleen32x64_mf);	
	//u8g2_SetFont(&u8g2, u8g2_font_fur35_tf);
	
	struct Key_data	key_data;	
	while(1)
	{
		u8g2_ClearBuffer(&u8g2);
		ShowClock();	// 调用时钟绘制函数	
		//u8g2_DrawXBMP(&u8g2, 0, 0, 20, 40, BigNum[temp]);
		u8g2_SendBuffer(&u8g2);
		
		// 读取按键消息队列
		if(clock_flag == 0)// clock_flag=0表示"设置模式"
		{
			// 从队列接收按键数据，无限等待直到有数据（pdPASS用于判断接收成功，此处未处理失败）
			pdPASS == xQueueReceive(g_xQueueMenu, &key_data, portMAX_DELAY);
		}
		// 处理右键（rdata=1：切换设置项，向右循环）
		if(key_data.rdata == 1)
		{
			//buzzer_buzz(2500, 100);
			seclect_flag++;
			if(seclect_flag>4)// 超过最大索引4（0-4共5项）则重置为0
				seclect_flag=0;
		}
		// 处理左键（ldata=1：切换设置项，向左循环）
		if(key_data.ldata == 1)
		{
			//buzzer_buzz(2500, 100);
			seclect_flag--;
			if(seclect_flag<0)// 小于0则重置为最大索引4
				seclect_flag=4;
		}
		// 处理上键（updata=1：调整时间或启动定时）
		if(key_data.updata == 1)
		{
			//buzzer_buzz(2500, 100);
			if(seclect_flag == 4)// 若当前选中第5项（索引4，可能是"启动定时"选项）
			{
				// 启动定时器
				if(g_Clock_Timer != NULL)
				{
					xTimerStart(g_Clock_Timer, 0);// 启动定时器（立即生效，无阻塞）
					clock_flag = 1;// 标记为"定时运行中"（clock_flag=1）
					key_data.updata = 0;// 清除按键状态（避免重复处理）
				}
			}
			else{// 若选中的是时间位（0-3：时高位、时低位、分高位、分低位）
				g_clock_num[seclect_flag]++;// 当前选中的时间数字+1（如0→1，8→9）
				if(g_clock_num[seclect_flag]>9)g_clock_num[seclect_flag]=0;// 超过9则重置为0				
			}
		}		
		// 处理确认键（exdata=1：返回主菜单）
		if(key_data.exdata == 1)
		{
			//buzzer_buzz(2500, 100);
			clock_flag = 0;
			xTimerStop(g_Clock_Timer, 0);
			vTaskResume(xShowMenuTaskHandle);
			vTaskSuspend(NULL);
		}	
		// 定时运行逻辑
		if(clock_flag == 1)
		{
			// 检查设置时间是否与实时时间一致（定时到达条件）
			if(g_clock_num[0]==g_real_time[0]&&g_clock_num[1]==g_real_time[1]&&g_clock_num[2]==g_real_time[2]&&g_clock_num[3]==g_real_time[3])
			{
				clock_flag = 0;// 重置为"设置模式"
				xTimerStop(g_Clock_Timer, 0);// 停止定时器
				/* music */
				//buzzer_buzz(2500, 1000);
			}
		}
	}
}

/****
		* @brief  定时更新实时时间（毫秒、分、时的递增与进位）
		* @param  TimerHandle_t xTimer：触发该回调的定时器句柄
		* @retval 无
*****/
void ClockTimerCallBackFun(TimerHandle_t xTimer)
{  
	millisecond++; // 毫秒计数器递增
	if(millisecond>9)// 当毫秒计数器超过9（累计10次触发），表示1秒时间到
	{
		millisecond = 0;// 重置毫秒计数器（重新开始计数）		
		g_real_time[3]++;// 分低位数字递增（如"12:34"中的"4"）
		if(g_real_time[3]>9)// 若分低位超过9（如4→10），则分低位清零，分高位递增
		{
			g_real_time[2]++;
			g_real_time[3]=0;
			if(g_real_time[2]>5)// 若分高位超过5（分钟最大为59，分高位最大是5）
			{
				g_real_time[1]++;
				g_real_time[2] = 0;
				if(g_real_time[1]>9)// 若时低位超过9（如2→10）
				{
					g_real_time[0]++;
					g_real_time[1] = 0;
					// 注意：此处未处理时高位超过2的情况（如23时→24时），可能需要根据需求补充（如24小时制归零）
				}
			}
		}	
	}
}
