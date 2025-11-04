#include "ShowFlashLight.h"

/*----------------------------------外部变量----------------------------------*/
extern QueueHandle_t g_xQueueMenu;
extern TaskHandle_t xShowMenuTaskHandle;

/****
		* @brief  控制手电筒功能的UI显示与交互
		* @param  params：任务创建时传递的参数
		* @retval 无
*****/
void ShowFlashLightTask(void *params)
{
	//buzzer_init();
	// 创建队列
	g_xQueueMenu = xQueueCreate(1, 4);
	// 若队列创建成功，点亮GPIOC引脚13
	if(NULL != g_xQueueMenu)HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	// 初始化u8g2显示配置
	u8g2_t u8g2;
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0, u8x8_byte_hw_i2c, u8g2_stm32_delay);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_ClearDisplay(&u8g2);
	//8g2_SetFont(&u8g2, u8g2_font_wqy16_t_chinese1);
	//8g2_SetFont(&u8g2, u8g2_font_spleen32x64_mf);	
	u8g2_SetFont(&u8g2, u8g2_font_fur35_tf);
	//u8g2_ClearBuffer(&u8g2);
	// 初始绘制手电筒图标：在坐标(48,16)处绘制30x30像素的light图标（手电筒关闭状态的视觉反馈）
	u8g2_DrawXBMP(&u8g2, 48, 16, 30, 30, light);
	u8g2_SendBuffer(&u8g2);
	// 定义手电筒状态标记：0=关闭（显示图标），1=开启（全屏亮）
	uint8_t light_flag = 0;
	struct Key_data	key_data;
	
	while(1)
	{
		u8g2_ClearBuffer(&u8g2);// 清除显示缓存（避免画面残留）	
		// 读按键中断队列
		xQueueReceive(g_xQueueMenu, &key_data, portMAX_DELAY);
		// 处理上键（updata=1表示上键被按下：控制手电筒开关切换）
		if(key_data.updata == 1)
		{
			//buzzer_buzz(2500, 100);
			switch(light_flag)
			{
				case 0:// 当前状态：关闭→切换到开启 
					u8g2_DrawBox(&u8g2, 0, 0, 128, 64);// 绘制全屏方块（填充整个屏幕，视觉上模拟“手电筒亮”）
					light_flag++;
					break;
				case 1:// 当前状态：开启→切换到关闭 
					u8g2_DrawXBMP(&u8g2, 48, 16, 30, 30, light);// 重新绘制手电筒图标（视觉上模拟“手电筒关”）
					light_flag--;
					break;
			}
		}		
		// 处理确认键（exdata=1表示确认键被按下：返回主菜单）
		if(key_data.exdata == 1)
		{
			//buzzer_buzz(2500, 100);
			vTaskResume(xShowMenuTaskHandle);// 唤醒主菜单任务（ShowMenuTask）
			vTaskSuspend(NULL);// 暂停当前手电筒任务（自身）
		}		
		u8g2_SendBuffer(&u8g2);// 将缓存中的最新画面（开关状态）发送到屏幕显示
	}
}
