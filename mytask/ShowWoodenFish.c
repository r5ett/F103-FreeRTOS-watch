#include "ShowWoodenFish.h"

/*----------------------------------外部变量----------------------------------*/
extern SemaphoreHandle_t g_xSemKey; 
extern TaskHandle_t xShowMenuTaskHandle;
extern QueueHandle_t g_xQueueMenu;
extern u8g2_t u8g2;

uint16_t WfisTaskRuning;

/****
		* @brief  木鱼游戏任务函数：实现木鱼敲击动画、功德计数、按键交互及界面刷新
		* @param  params：任务创建时传递的参数
		* @retval 无
*****/
void ShowWoodenFishTask(void *params)
{
	//xSemaphoreTake(g_xSemKey, portMAX_DELAY);
	
	// 系统声音初始化
	//buzzer_init();
	// 创建队列
	g_xQueueMenu = xQueueCreate(4, 4);
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
	// 木鱼状态标志（0=就绪状态）、动画选择标志（0=默认态，1=敲击动画态）、+1提示显示标志（控制提示持续时间）
	uint8_t wooden_fish_flag = 0, seclect_flag = 0, add_flag = 0;
	// 功德计数（3位数字）：num1=百位，num2=十位，num3=个位（如num1=1、num2=2、num3=3表示123功德）
	uint8_t num1 = 0, num2 = 0, num3 = 0; 
	
	while(1)
	{
		WfisTaskRuning = 1;// 标记木鱼任务正在运行
		u8g2_ClearBuffer(&u8g2);
		// 绘制功德计数（右上角显示3位数字）
		u8g2_DrawXBMP(&u8g2, 80, 0, 6, 8, Num_6x8[num1]);		
		u8g2_DrawXBMP(&u8g2, 88, 0, 6, 8, Num_6x8[num2]);		
		u8g2_DrawXBMP(&u8g2, 96, 0, 6, 8, Num_6x8[num3]);		
		// 根据动画标志（seclect_flag）绘制木鱼和锤子的不同状态
		switch(seclect_flag)
		{
			case 0:// case0：默认状态（未敲击） 
				u8g2_DrawXBMP(&u8g2, 8, 15, 63, 48, wooden_flsh[0]);// 绘制木鱼默认位图
				u8g2_DrawXBMP(&u8g2, 5, 0, 30, 8, hammer);// 绘制锤子默认位图
				break;
      case 1:// case1：敲击动画状态（触发后显示一次动画）
				u8g2_DrawXBMP(&u8g2, 8, 15, 49, 38, wooden_flsh[1]);// 绘制木鱼敲击动画位图
				u8g2_DrawXBMP(&u8g2, 10, 8, 30, 8, hammer);// 绘制锤子敲击位置位图
				seclect_flag = 0;// 动画显示后立即重置为默认态
				break;
		}
		// 绘制"功德"字样位图
		u8g2_DrawXBMP(&u8g2, 75, 37, 32, 16, gongde);
		u8g2_DrawStr(&u8g2, 110, 47, ":");// 绘制功德计数分隔符":"			
		u8g2_SendBuffer(&u8g2);
		
		// 读按键中断队列
		if(wooden_fish_flag == 0 && seclect_flag == 0)// 木鱼就绪且无动画时，才接收按键
		{
			pdPASS == xQueueReceive(g_xQueueMenu, &key_data, portMAX_DELAY);
		}
		// 处理右键（rdata=1：敲击木鱼）
		if(key_data.rdata == 1)
		{
			//buzzer_buzz(3000, 200);
			num3++;// 功德计数个位+1（如3→4，9→10）
			if(num3>9){num3=0;num2++;}// 个位满9进1，个位重置为0，十位+1
			if(num2>9){num2=0;num1++;}// 十位满9进1，十位重置为0，百位+1（最大计数999）
			seclect_flag = 1;// 触发木鱼敲击动画（切换到case1）
			add_flag = 100;// 设置"+1"提示显示次数（持续显示100帧）
			key_data.rdata = 0;// 清除右键状态（避免重复敲击）
			key_data.exdata = 0;// 清除确认键状态（防误触）
		}		
		// 处理确认键（exdata=1：返回主菜单） 
		if(key_data.exdata == 1)
		{
			//buzzer_buzz(2500, 100);
			vTaskResume(xShowMenuTaskHandle);
			vTaskSuspend(NULL);
		}		
		u8g2_ClearBuffer(&u8g2);
		if(add_flag!=0){// 若"+1"提示标志未归零，递减计数并绘制"+1"文字（持续显示）
			add_flag--;// 每帧减1，直到0（控制提示显示时长）
			u8g2_DrawStr(&u8g2, 105, 40, "+1");// 绘制敲击加分提示"+1"（X=105,Y=40）
		}
		// 重新绘制木鱼和锤子状态（与上一帧一致，确保动画连贯性）
		switch(seclect_flag)
		{
			case 0: 
				u8g2_DrawXBMP(&u8g2, 8, 15, 63, 48, wooden_flsh[0]);
				u8g2_DrawXBMP(&u8g2, 5, 0, 30, 8, hammer);
				break;
      case 1: 
				u8g2_DrawXBMP(&u8g2, 8, 15, 49, 38, wooden_flsh[1]);
				u8g2_DrawXBMP(&u8g2, 10, 8, 30, 8, hammer);
				seclect_flag = 0;
				break;
		}			
		u8g2_SendBuffer(&u8g2);
	}
}
