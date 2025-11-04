#include "ShowMenu.h"

/*----------------------------------外部变量----------------------------------*/
extern u8g2_t u8g2;
extern QueueHandle_t g_xQueueMenu; 
extern TaskHandle_t xShowMenuTaskHandle;
extern TaskHandle_t xShowTimeTaskHandle;
extern TaskHandle_t xShowWoodenFishTaskHandle;
extern TaskHandle_t xShowFlashLightTaskHandle;
extern TaskHandle_t xShowSettingTaskHandle;
extern TaskHandle_t xShowClockTaskHandle;
extern TaskHandle_t xShowCalendarTaskHandle;
extern TaskHandle_t xShowDHT11TaskHandle;

const char str[5][10] = {"cleder", "torch", "hum", "clock", "more"};

// APP名
str1 fly1			= {"fly1", NULL}		;
str1 dino1		= {"hum", NULL}			;
str1 test1		= {"torch", NULL}		;
str1 block1		= {"clock", NULL}		;
str1 setting1 = {"setting", NULL}	;

// 其他数据
Image Left 				= {0, 0, 23, 10}	;
Image Right	 			= {104, 0, 23, 10};
Image String 			= {53, 10, 0, 0}	;
Image Rec_select 	= {49, 16, 32, 32};
uint8_t dock_pos = 2;// 底部导航栏中实心的圆形导航点 0~4初始值为2
uint8_t dock_status = 10;//选中导航点切换时的图标动画效果
uint8_t dock[5] = {45, 55, 65, 75, 85};// 存储5 个导航点的水平（X）坐标
uint8_t dock_y = 58;// 导航点的垂直（Y）坐标
uint8_t dock_r = 3;// 导航点的圆形半径
int str_flag = 2;// 当前选中应用的文本索引 0~4初始值为2
int8_t R_move_pos[5] = {-1, 9, 49, 89, 129};// 存储 5 个应用对应的水平（X）目标位置，用于导航切换时的图标移动动画
BaseType_t select = 3;// 标记某个层级的选中项索引（可能是应用内的子选项，或全局的次级选中状态）
int queue_flag = 0;//控制按键消息处理与UI动画的同步节奏
uint32_t end_flag = 1;//0标记动画开始（结束状态置0），1标记不开始
uint32_t seclect_end = 0;//设置界面（ShowSetting）的选中状态确认或退出触发

/****
		* @brief  绘制整个系统的UI
		* @param  无
		* @retval 无
*****/
void ShowUI(void)
{
	// 左右图标
	u8g2_DrawXBMP(&u8g2, Left.x, Left.y, Left.w, Left.h, LeftMove);
	u8g2_DrawXBMP(&u8g2, Right.x, Right.y, Right.w, Right.h, RightMove);
	// 当前选中应用名称
	u8g2_DrawStr(&u8g2, String.x, String.y, str[str_flag]);
	// 5 个应用图标
	u8g2_DrawXBMP(&u8g2, cleder.x, cleder.y, cleder.w, cleder.h, cleder.data);
	u8g2_DrawXBMP(&u8g2, torch.x, torch.y, torch.w, torch.h, torch.data);	
	u8g2_DrawXBMP(&u8g2, hum.x, hum.y, hum.w, hum.h, hum.data);
	u8g2_DrawXBMP(&u8g2, clock.x, clock.y, clock.w, clock.h, clock.data);
	u8g2_DrawXBMP(&u8g2, setting.x, setting.y, setting.w, setting.h, setting.data);
	// 底部栏(5个⚪)
	u8g2_DrawDisc(&u8g2, dock[dock_pos], dock_y, dock_r, U8G2_DRAW_ALL);
	for(int i = 0; i<5; i++)
	{
		u8g2_DrawCircle(&u8g2, dock[i], dock_y, dock_r, U8G2_DRAW_ALL);
	}	
	// 选择框
	u8g2_DrawFrame(&u8g2, Rec_select.x, Rec_select.y, Rec_select.w, Rec_select.h);
};

/****
		* @brief  菜单UI显示、交互及任务调度
		* @param  params：任务创建时传递的参数
		* @retval 无
*****/
void ShowMenuTask(void *params)
{
//	xSemaphoreTake(g_xQueueMenu, portMAX_DELAY);
	
	// 系统声音初始化
	//buzzer_init();
   
	//创建队列
	g_xQueueMenu = xQueueCreate(4, 4);
	// 若队列创建成功，点亮GPIOC引脚13
	if(NULL != g_xQueueMenu)HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);	
	u8g2_config();
	// 显示UI
	//u8g2_SetFont(&u8g2, u8g2_font_wqy16_t_chinese1);
	u8g2_FirstPage(&u8g2);// u8g2库分页显示：开始第一页
	do{
		u8g2_SendBuffer(&u8g2);// 将缓存内容发送到屏幕（初始刷新）
  } while (u8g2_NextPage(&u8g2));// 循环直到所有分页显示完成（此处仅一页）
	// 定义按键数据结构体变量
	struct Key_data	key_data;
	// 任务主循环（FreeRTOS任务核心，无限循环执行）
	while(1)
	{
		u8g2_ClearBuffer(&u8g2);// 清除显示缓存（避免画面残留）
		ShowUI();// 绘制菜单UI（调用ShowMenu.c中的绘制函数，包含图标、导航栏等）
		u8g2_SendBuffer(&u8g2);// 将缓存中的UI内容发送到屏幕硬件（刷新显示）
		// 接收队列数据并等待（阻塞式）
		if(queue_flag == 0)// 若动画未执行（queue_flag用于控制动画节奏）
		{
			// 从队列接收按键数据，一直等待直到有数据（portMAX_DELAY表示无限等待）
			pdPASS == xQueueReceive(g_xQueueMenu, &key_data, portMAX_DELAY);
		}
		// 处理按键数据：右键（rdata=1表示右键被按下）
		if(key_data.rdata == 1)
		{	
			end_flag = 0;// 标记动画开始（结束状态置0）
			//if(end_flag!)
			// 若当前导航点位置不是最左侧（0），执行向右移动动画
			if(dock_pos != 0)
			{
				// 所有应用图标向右移动（步长2），实现切换动画
				ui_right(&cleder.x, 2);
				ui_right(&torch.x, 2);
				ui_right(&hum.x, 2);
				ui_right(&clock.x, 2);
				ui_right(&setting.x, 2);
				
				// 状态机：控制图标移动动画
				if(dock_status==0)dock_status=1;// 重置动画状态（避免异常值）
				dock_status--;// 动画步数递减（控制动画进度）
				// 根据当前导航点位置，调整图标上下移动（增强切换视觉效果）
				switch(dock_pos)
				{
					case 2:// 当前在第3个导航点
						if(dock_status!=0){
							ui_up(&cleder.y, 1);
							ui_up(&torch.y, 1);
							ui_down(&hum.y, 1);
							ui_down(&clock.y, 1);
						}
						break;
				  case 1:// 当前在第2个导航点
						if(dock_status!=0){
							ui_up(&cleder.y, 1);
							ui_up(&setting.y, 1);
							ui_down(&torch.y, 1);
							ui_down(&hum.y, 1);
						}break;
					case 4:// 当前在第5个导航点
						if(dock_status!=0){
							ui_up(&clock.y, 1);
							ui_up(&hum.y, 1);
							ui_down(&setting.y, 1);
							ui_down(&cleder.y, 1);
						}break;
					case 3:// 当前在第4个导航点
						if(dock_status!=0){
							ui_up(&hum.y, 1);
							ui_up(&torch.y, 1);
							ui_down(&setting.y, 1);
							ui_down(&clock.y, 1);
						}break;
				}
			}
      queue_flag++;// 动画步数递增（记录当前动画进度）
			if(queue_flag == 20)// 若动画执行到20步（结束条件）
			{
				dock_status=10;// 重置动画状态变量
				end_flag = 1;// 标记动画结束
				// 导航点左移（切换到前一个应用），更新选中的文本索引
				if(dock_pos != 0){
					dock_pos--;// 导航点
					str_flag--;// 文本索引
				}
				queue_flag = 0;// 重置动画进度
				key_data.rdata = 0;// 清除按键状态（避免重复处理）
				key_data.ldata = 0;
			}
			// 动画结束后，蜂鸣器响100ms（频率2000Hz）作为反馈
//			if(end_flag == 1)
//				//buzzer_buzz(2000, 100);
		}
		// 处理按键数据：左键（ldata=1表示左键被按下）
		else if(key_data.ldata == 1)
		{
			end_flag = 0;// 标记动画开始
			// 若当前导航点位置不是最右侧（4），执行向左移动动画
			if(dock_pos < 4)
			{
				// 所有应用图标向左移动（步长2），实现切换动画
				ui_left(&cleder.x, 2);
				ui_left(&torch.x, 2);
				ui_left(&hum.x, 2);
				ui_left(&clock.x, 2);
				ui_left(&setting.x, 2);
				// 状态机：控制图标移动动画
				if(dock_status==0)
					dock_status=1;// 重置动画状态
				dock_status--;// 动画步数递减
				// 根据当前导航点位置，调整图标上下移动
				switch(dock_pos)
				{
					case 0:
						if(dock_status!=0){
						ui_up(&hum.y, 1);
						ui_up(&torch.y, 1);
						ui_down(&cleder.y, 1);
						ui_down(&setting.y, 1);
						}break;
					case 1:
						if(dock_status!=0){
						ui_up(&hum.y, 1);
						ui_up(&clock.y, 1);
						ui_down(&torch.y, 1);
						ui_down(&cleder.y, 1);
						}break;
					case 2:
						if(dock_status!=0){
						ui_up(&clock.y, 1);
						ui_up(&setting.y, 1);
						ui_down(&hum.y, 1);
						ui_down(&torch.y, 1);
						}break;
					case 3:
						if(dock_status!=0){
						ui_up(&setting.y, 1);
						ui_up(&cleder.y, 1);
						ui_down(&clock.y, 1);
						ui_down(&hum.y, 1);
						}break;
				}				
			}
			queue_flag++;	
			// 若动画执行到20步（结束条件），重置数据
			if(queue_flag == 20) 
			{
				dock_status = 10;
				end_flag = 1;
				if(dock_pos < 4){dock_pos++;str_flag++;}
				queue_flag = 0;
				key_data.ldata = 0;
				key_data.rdata = 0;
			}
				// 动画结束后，蜂鸣器响100ms作为反馈
	//			if(end_flag == 1)
	//				//buzzer_buzz(2000, 100);
		}
			// 处理按键数据：确认键（exdata=1表示确认键被按下）
		else if(key_data.exdata == 1)
		{
			//buzzer_buzz(2000, 100);// 蜂鸣器反馈
			switch(dock_pos)// 根据当前导航点位置，切换到对应的子任务（如日历、手电筒等）
			{
				case 0:// 第1个导航点：唤醒日历任务，暂停当前菜单任务 
					vTaskResume(xShowCalendarTaskHandle);
					vTaskSuspend(NULL);// 暂停自身（当前任务）
					key_data.exdata = 0;// 清除按键状态
					break;
				case 1:// 第2个导航点：唤醒手电筒任务 
					vTaskResume(xShowFlashLightTaskHandle);
					vTaskSuspend(NULL);
					key_data.exdata = 0;
					break;
				case 2:// 第3个导航点：唤醒DHT11（温湿度）任务 
					vTaskResume(xShowDHT11TaskHandle);
					vTaskSuspend(NULL);
					key_data.exdata = 0;
					break;
				case 3:// 第4个导航点：唤醒时钟任务 
					vTaskResume(xShowClockTaskHandle);
					vTaskSuspend(NULL);
					key_data.exdata = 0;
					break;
				case 4:// 第5个导航点：唤醒设置任务 
					vTaskResume(xShowSettingTaskHandle);
					vTaskSuspend(NULL);
					key_data.exdata = 0;
					break;
			}
		}
		// 处理按键数据：上键（updata=1表示上键被按下）
		else if(key_data.updata == 1)
		{
			// 按键声音反馈
			//buzzer_buzz(2000, 100);
			// 唤醒时间显示任务，暂停当前菜单任务
			vTaskResume(xShowTimeTaskHandle);
			vTaskSuspend(NULL);// 暂停自身
			key_data.updata = 0;// 清除按键状态
		}
	}
}
