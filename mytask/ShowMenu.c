#include "ShowMenu.h"

/*----------------------------------外部变量----------------------------------*/
extern u8g2_t u8g2;

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
uint32_t end_flag = 1;//控制某个任务或功能模块的运行状态
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






































