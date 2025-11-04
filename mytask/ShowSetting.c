#include "ShowSetting.h"

/*----------------------------------外部变量----------------------------------*/
extern TaskHandle_t xShowMenuTaskHandle;
extern QueueHandle_t g_xQueueMenu;
extern u8g2_t u8g2;
extern BaseType_t end_flag;
extern BaseType_t seclect_end;

const char strs[5][10] = {"<<<", "record", "Sound", "Power", "About"}; 
BaseType_t str_x_pos = 1;// 字符串（文本）的X轴起始坐标（像素位置）
BaseType_t str_y_pos[] = {11, 23, 36, 49, 62};// 字符串（文本）的Y轴坐标数组
BaseType_t about_x_pos = 55;// "about"中元素的X轴起始坐标
BaseType_t about_y_pos[] = {13, 27, 41, 55};// "about"中元素的Y轴坐标数组（共4个元素），对应4行内容的垂直位置

int32_t seclect = 0;// 选中索引
int32_t seclect_h = 13;// 选中框的高度
int32_t seclect_y[6] = {0, 13, 25, 38, 51, 0};// 选中框的Y轴坐标数组（共6个元素）
int32_t seclect_w[6] = {24, 44, 37, 37, 37, 24};// 选中框的宽度（width）数组（共6个元素）
int width[5] = {0};
int power_button = 0;// 控制系统声音

/****
		* @brief  Setting绘制函数
		* @param  无
		* @retval 无
*****/
void ShowSetiing(void)
{
	// 循环绘制5个设置选项
	for(int i = 0; i<5; i++)
	{
		u8g2_DrawStr(&u8g2, str_x_pos, str_y_pos[i], strs[i]);
		width[i] = u8g2_GetStrWidth(&u8g2, &strs[i][10]);// 计算第i个设置选项文本中“从第10个字符开始的子串”的宽度，并存入width数组
		//u8g2_GetMaxCharHeight();		
	}
	u8g2_DrawFrame(&u8g2, 0, seclect_y[0], seclect_w[0], seclect_h);// 绘制第0个设置选项的选中边框（空心矩形）		
	u8g2_DrawBox(&u8g2, 48, seclect_y[0], 5, 12);// 绘制一个实心矩形
	u8g2_DrawFrame(&u8g2, 48, 0, 5, 63);// 绘制一条垂直分隔边框	
}

/****
		* @brief  About绘制函数
		* @param  无
		* @retval 无
*****/
void ShowAbout(void)
{
	u8g2_DrawStr(&u8g2, about_x_pos, about_y_pos[0],  "thank you");
	u8g2_DrawStr(&u8g2, about_x_pos, about_y_pos[1],  "following");
	u8g2_DrawStr(&u8g2, about_x_pos, about_y_pos[2],  "my project");
	u8g2_DrawStr(&u8g2, about_x_pos, about_y_pos[3],  "@r5ett");
}

/****
		* @brief  开关控件绘制函数绘制函数
		* @param  int switch_status：开关状态（0=开启状态，1=关闭状态）
		* @retval 无
*****/
void ShowSwitch(int switch_status)
{
	// 绘制开关上边框：水平直线（X=70，Y=22，长度40像素）
  // 作用：构成开关的上边缘，限定开关的水平范围（70~110像素）
	u8g2_DrawHLine(&u8g2, 70, 22, 40);
	// 绘制开关下边框：水平直线（X=70，Y=40，长度40像素）
  // 作用：构成开关的下边缘，与上边框配合形成开关的矩形轮廓（高度18像素）
	u8g2_DrawHLine(&u8g2, 70, 40, 40);
	// 当开关状态为0（开启状态）时，绘制开启样式
	if(switch_status == 0)
	{
		// 绘制开关滑块（实心圆）,位置：开关左侧
		u8g2_DrawDisc(&u8g2, 70, 31, 9, U8G2_DRAW_ALL);
		// 绘制"on"状态文字
		u8g2_DrawStr(&u8g2, 100, 34,  "on");  
		// 绘制开关右侧半圆边框
		u8g2_DrawCircle(&u8g2, 110, 31, 9, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_LOWER_RIGHT);		
	}
	if(switch_status == 1)
	{
		// 绘制开关滑块（实心圆）,位置：开关右侧
		u8g2_DrawDisc(&u8g2, 110, 31, 9, U8G2_DRAW_ALL);
		// 绘制"off"状态文字
		u8g2_DrawStr(&u8g2, 69, 36,  "off"); 
		// 绘制开关左侧半圆边框
		u8g2_DrawCircle(&u8g2, 70, 31, 9, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_LOWER_LEFT);
	}
}

/****
		* @brief  管理设置界面的绘制、选项交互、开关控制及任务调度
		* @param  params：任务创建时传递的参数
		* @retval 无
*****/
void ShowSetting_Task(void)
{
	//buzzer_init();
	// 创建队列
	g_xQueueMenu = xQueueCreate(4, 4);
	if(NULL != g_xQueueMenu)HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

	u8g2_config();
	u8g2_SetFont(&u8g2, u8g2_font_7x13_mf);	
  u8g2_FirstPage(&u8g2);// u8g2分页绘制初始化
	do{
		ShowSetiing();// 调用设置选项绘制函数（画5个设置选项、选中框、垂直分隔线）
		u8g2_SendBuffer(&u8g2);
  }while(u8g2_NextPage(&u8g2));// 分页绘制循环（此处仅需1页，完成初始界面绘制）
	// 循环计算5个设置选项中“可变内容”的宽度（&strs[i][10]取文本第10位后的子串，如参数值）
  // 结果存入width数组，用于后续适配绘制背景框或对齐文本
	for(int i = 0; i<5; i++)
	{
		width[i] = u8g2_GetStrWidth(&u8g2, &strs[i][10]);
	}
	struct Key_data	key_data;// 定义按键数据结构体（存储接收的按键消息）
	// 任务主循环（持续刷新界面、处理交互）
	while(1)
	{
		u8g2_ClearBuffer(&u8g2);
		// 根据当前选中索引（seclect），绘制对应选项的详情内容 		
		switch(seclect)
		{
			case 0:// 选中第0个选项：显示作者信息和日期 
				u8g2_DrawStr(&u8g2, 64, 25,"@r5ett");
				u8g2_DrawStr(&u8g2, 61, 50,"2025/11/04");
				break;
			case 1:// 选中第1个选项：显示开关控件 
				ShowSwitch(power_button);
				break;
			case 2:// 选中第2个选项：显示开关控件 
				ShowSwitch(power_button);
				break;
			case 3:// 选中第3个选项：显示开关控件 
				ShowSwitch(power_button);
				break;
			case 4:// 选中第4个选项：调用“关于”页面绘制函数（显示设备/版本信息） 
				ShowAbout();
				break;
		}
		ShowSetiing();// 重新绘制设置选项列表+选中框（覆盖详情内容的背景，确保选项和选中状态不被覆盖）
		u8g2_SendBuffer(&u8g2);
		if(seclect_end == 0) // 仅在“未处于选项切换中”（seclect_end=0）时
		{
			// 从队列接收按键数据，无限阻塞等待（确保按键不丢失），pdPASS仅用于判断接收成功（此处未处理失败）
			pdPASS == xQueueReceive(g_xQueueMenu, &key_data, portMAX_DELAY);
    }
		// 处理右键：向下切换设置选项
		if(key_data.rdata == 1)
		{
			seclect_end++;// 切换计数递增
			if(seclect!=4)// 若当前未选中最后一个选项（seclect≠4），则更新选中框的尺寸
			{
				// 状态机：根据当前选中索引，调用ui_run更新选中框的宽度和Y坐标
				switch(seclect)
				{
					case 0:// 当前选中第0项→切换到第1项：更新选中框宽为seclect_w[1]，Y为seclect_y[1] 
						ui_run(&seclect_w[0], &seclect_w[1], 1);
						ui_run(&seclect_y[0], &seclect_y[1], 1);
						break;
					case 1:// 当前选中第1项→切换到第2项：更新选中框宽为seclect_w[2]，Y为seclect_y[2] 
						ui_run(&seclect_w[0], &seclect_w[2], 1);
						ui_run(&seclect_y[0], &seclect_y[2], 1);
						break;
					case 2:// 当前选中第2项→切换到第3项：更新选中框宽为seclect_w[3]，Y为seclect_y[3] 
						ui_run(&seclect_w[0], &seclect_w[3], 1);
						ui_run(&seclect_y[0], &seclect_y[3], 1);
						break;
					case 3:// 当前选中第3项→切换到第4项：更新选中框宽为seclect_w[4]，Y为seclect_y[4] 
						ui_run(&seclect_w[0], &seclect_w[4], 1);
						ui_run(&seclect_y[0], &seclect_y[4], 1);
						break;
					case 4:// 当前选中最后一项→不切换（冗余判断，上方已限制seclect≠4）
						ui_run(&seclect_w[0], &seclect_w[5], 1);
						ui_run(&seclect_y[0], &seclect_y[5], 1);
						break;				
				}
			}
			if(seclect_end == 20)// 当切换计数累加到20，执行选项切换
			{
				if(seclect!=4)seclect++;// 未选中最后一项则索引+1（切换到下一项）
				seclect_end = 0;// 重置切换计数
				key_data.rdata = 0;// 清除按键状态
			}
			//if(seclect_end == 0)buzzer_buzz(2500, 100);			                     
		}
		// 处理左键：向上切换设置选项，同理
		else if(key_data.ldata == 1)
		{
			seclect_end++;
			if(seclect!=0)
			{
				switch(seclect)
				{
					case 0: 
						break;
					case 1: 
						ui_run(&seclect_w[0], &seclect_w[5], 1);
						ui_run(&seclect_y[0], &seclect_y[5], 1);
						break;
					case 2: 
						ui_run(&seclect_w[0], &seclect_w[1], 1);
						ui_run(&seclect_y[0], &seclect_y[1], 1);
						break;
					case 3: 
						ui_run(&seclect_w[0], &seclect_w[2], 1);
						ui_run(&seclect_y[0], &seclect_y[2], 1);
						break;
					case 4: 
						ui_run(&seclect_w[0], &seclect_w[3], 1);
						ui_run(&seclect_y[0], &seclect_y[3], 1);
						break;				
				}				
			}
			if(seclect_end == 20)
			{
				if(seclect!=0)seclect--;
				seclect_end = 0;
				key_data.ldata = 0;
			}
			//if(seclect_end == 0)buzzer_buzz(2500, 100);			                     			
		}
		// 处理上键（updata=1）：控制当前选中选项的开关状态切换
		if(key_data.updata == 1)
		{
			//buzzer_buzz(2500, 100);			                     
			power_button++;// 开关状态标记+1（0→1，1→2）
			power_button = power_button%2;// 取模2，实现0（开启）和1（关闭）的循环切换
		}		
		// 处理确认键（exdata=1）：返回主菜单
		if(key_data.exdata == 1)
		{
			//buzzer_buzz(2500, 100);			                     
			vTaskResume(xShowMenuTaskHandle);
			vTaskSuspend(NULL);
		}
	}
}
