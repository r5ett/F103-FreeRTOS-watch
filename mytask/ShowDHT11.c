#include "ShowDHT11.h"

/*----------------------------------外部变量----------------------------------*/
extern TaskHandle_t xShowMenuTaskHandle;
extern QueueHandle_t g_xQueueMenu;

/****
		* @brief  DHT11 温湿度监测任务
		* @param  params：任务创建时传递的参数
		* @retval 无
*****/
void ShowDHT11Task(void *params)
{
	DHT11_Init();
	//buzzer_init();
	// 创建队列
	g_xQueueMenu = xQueueCreate(1, 4);
	if(NULL != g_xQueueMenu)HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

	u8g2_t u8g2;
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0, u8x8_byte_hw_i2c, u8g2_stm32_delay);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_ClearDisplay(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_wqy16_t_chinese1);
	u8g2_SendBuffer(&u8g2);
	
	struct Key_data	key_data;
	int hum, temp;// 存储DHT11读取的原始数据：hum=湿度值（0-99%RH），temp=温度值（0-99℃）
  int hum1, hum2, hum3, temp1, temp2 ,temp3;// 温湿度拆分后的数位：用于大数字显示（如temp2=十位，temp1=个位）
  int max;// 临时变量：存储湿度参考阈值的最大值
	int g_max[] = {20, 30, 40, 50, 60, 70};// 湿度参考阈值数组
	
	while(1)
	{	
		u8g2_ClearBuffer(&u8g2);			
		if (DHT11_Read(&hum, &temp) !=0 ){
			//printf("\n\rdht11 read err!\n\r");
			DHT11_Init();// 读取失败时，重新初始化DHT11
		}
		else{// 数据读取成功，拆分温湿度为“十位+个位”
			temp1 = temp%20;	//low bit温度个位
			temp3 = temp/10;	// 温度十位
			temp2 = temp3%20; //high bit 温度十位
			// 从g_max数组中筛选“小于等于当前湿度hum”的最大阈值		
			for(int i=0; i<5; i++)
			{
				max = hum>g_max[i]?g_max[i]:max;
			}			
			hum1 = hum%max;		//low bit	
			hum3 = hum/10;
			hum2 = hum3%max;    //high bit
			// 绘制温度大数字
			u8g2_DrawXBMP(&u8g2, 10, 20, 20, 40, BigNum[temp2]);
			u8g2_DrawXBMP(&u8g2, 35, 20, 20, 40, BigNum[temp1]);
			// 绘制湿度大数字
			u8g2_DrawXBMP(&u8g2, 75, 20, 20, 40, BigNum[hum2]);
			u8g2_DrawXBMP(&u8g2, 100, 20, 20, 40, BigNum[hum1]);
		}
		// 绘制温湿度标签
		u8g2_DrawStr(&u8g2, 15, 15, "temp");
		u8g2_DrawStr(&u8g2, 85, 15, "Hum");
		u8g2_SendBuffer(&u8g2);
	
		// 读按键中断队列
		xQueueReceive(g_xQueueMenu, &key_data, 0);
		// 处理确认键
		if(key_data.exdata == 1)
		{
			//buzzer_buzz(2500, 100);
			vTaskResume(xShowMenuTaskHandle);
			vTaskSuspend(NULL);
			key_data.exdata = 0;
		}
	}
}
