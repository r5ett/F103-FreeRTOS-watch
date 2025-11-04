#include "ShowCalendar.h"

/*----------------------------------外部变量----------------------------------*/
extern TaskHandle_t xShowMenuTaskHandle;
extern QueueHandle_t g_xQueueMenu;


/****
		* @brief  判断是不是闰年
		* @param  int year：输入年份
		* @retval 1：闰年		0:不是闰年
*****/
int judge_year(int year){
	if((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)))
		return 1;
	else
		return 0;
}

/****
		* @brief  判断需要输出的年份的一月一日是星期几
		* @param  int year：输入年份
		* @retval 周一~周日
*****/
int judge_week(int year)
{
	if(year==1)
		return 1;
	int sum=0;
	for(int i=1; i<year; i++){
		if(judge_year(i)==1)
			sum=sum+366;
		else
			sum=sum+365;
	}
	return (sum+1)%7;
}

/****
		* @brief  闰年各个月份的天数
		* @param  int n：月份
		* @retval 天数
*****/
int month_run(int n){
	switch(n){
		case 1:return 31;
		case 2:return 29;
		case 3:return 31;
		case 4:return 30;
		case 5:return 31;
		case 6:return 30;
		case 7:return 31;      
		case 8:return 31;
		case 9:return 30;
		case 10:return 31;
		case 11:return 30;
		case 12:return 31;
		default: return 0;
	}
}

/****
		* @brief  平年各个月份的天数
		* @param  int n：月份
		* @retval 天数
*****/
int month_ping(int n){
	switch(n){
		case 1:return 31;
		case 2:return 28;
		case 3:return 31;
		case 4:return 30;
		case 5:return 31;
		case 6:return 30;
		case 7:return 31;
		case 8:return 31;
		case 9:return 30;
		case 10:return 31;
		case 11:return 30;
		case 12:return 31;
		default: return 0;
	}
}

/****
		* @brief  控制日历功能的UI显示与交互
		* @param  params：任务创建时传递的参数
		* @retval 无
*****/
void ShowCalendarTask(void *params)
{
	//buzzer_init();
	/* 创建队列 */
	g_xQueueMenu = xQueueCreate(1, 4);
	// 若队列创建成功，点亮GPIOC引脚13
	if(NULL != g_xQueueMenu)HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	// 初始化u8g2显示配置
	u8g2_t u8g2;
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2,U8G2_R0, u8x8_byte_hw_i2c, u8g2_stm32_delay);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_ClearDisplay(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_spleen5x8_mf);
	u8g2_SendBuffer(&u8g2);
	
	struct Key_data	key_data;
	// 日期字符串数组：索引0-31，对应日期"0"到"31"（0未使用，从1开始）
	const char ucMonthDay[32][3] = {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31"};	
	// 星期字符串数组：Su（周日）到Sa（周六）
	const char ucWeekHeader[7][3] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
	// 星期的X坐标数组：7个星期分别在屏幕X=0,17,...102的位置（均匀分布）
	uint16_t usWeekX[7] = {0,  17, 34 , 51, 68, 85, 102};
	// 日期行的Y坐标数组：6行日期分别在Y=17,26,...62的位置（每行间隔9像素）
	uint16_t usWeekY[6] = {17, 26, 35 ,44, 53, 62};
	// 月份选择线的Y坐标数组：12条线对应1-12月的选中位置
	uint16_t usLineY[12] = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 110, 121};
	// 当前选中的月份索引（0-11，对应1-12月）
	uint8_t line_pos  = 0;
	// 星期位置标记（用于计算日期的显示行）
	uint8_t week_pos  = 0;
	// 临时变量：存储星期计算结果、月份天数、日期换行判断
	uint32_t week_temp, week_temp_temp, month_temp, enter_temp;
	// 当前显示的月份（1-12）
	uint32_t month = 1;
	//记录当前月的第一天是星期几（0=周日，6=周六）
	uint16_t wee = 0;	
	
	while(1)
	{	
		// 清除显示缓存（避免画面残留）		
		u8g2_ClearBuffer(&u8g2);			
		// 绘制星期头（Su到Sa）：循环7次，按usWeekX坐标绘制到屏幕顶部（Y=8）	
		for(int i=0; i<=6; i++){
			u8g2_DrawStr(&u8g2, usWeekX[i], 8, ucWeekHeader[i]);			
		}		
		//计算当前月份的天数和第一天是星期几
		/*---------------------------------------------------------------需要优化---------------------------------------------------------------*/
		month_temp = month_run(month);//获取天数		
		week_temp = judge_week(2024);//获取1月1日是星期几				
		wee = week_temp;// 用wee暂存基准星期值
		// 计算当前月份第一天是星期几：累加前几个月的天数，对7取余（一周7天）
		for(int m=1; m<month; m++){
			wee = (wee+month_run(m))%7;
		}
		
		week_temp = wee;// 存储当前月份第一天的星期几（0=周日）		
		week_temp_temp = week_temp;// 临时保存，用于判断日期所在行
		
		// 绘制当前月的日历
		for(int k=1; k<=month_temp; k++){// k从1循环到当月天数（如31天）		
			enter_temp  = week_temp%7;// 计算当前日期是星期几（0=周日，6=周六）
			week_temp++;// 下一个日期的星期数+1		
			// 判断当前日期所在的行（usWeekY的索引）
			if(k<=(7-week_temp_temp)){// 当月前几天（1-7-第一天星期数）在第0行
				week_pos=0;
			}else if(enter_temp == 0){// 遇到周日（enter_temp=0）则换行到下一行
				week_pos = week_pos+1;			
			}
			// 在计算好的坐标（usWeekX[星期]，usWeekY[行]）绘制日期字符串
			u8g2_DrawStr(&u8g2, usWeekX[enter_temp], usWeekY[week_pos], ucMonthDay[k]);	
		}
		// 绘制右侧月份选择分隔线：X=115处画一条垂直线（分隔日历主体和月份选择区）
		u8g2_DrawLine(&u8g2, 115, 0, 115, 62);
		// 在右侧显示当前选中的月份（line_pos+1对应1-12月）
		u8g2_DrawStr(&u8g2, 117, 32, ucMonthDay[line_pos+1]);	
		// 绘制月份选中线：在usLineY[line_pos]处画一条水平线（标记当前选中的月份）
		u8g2_DrawHLine(&u8g2, usLineY[line_pos], 63, 11);
		
		u8g2_SendBuffer(&u8g2);

		// 读按键中断队列
		xQueueReceive(g_xQueueMenu, &key_data, portMAX_DELAY);
		//处理右键（rdata=1表示右键被按下：切换到下一个月）
		if(key_data.rdata == 1)
		{
			//buzzer_buzz(2500, 100);
			month++;// 当前月份+1（如1→2）
			line_pos++;// 月份选中线位置+1（切换到下一个月的标记）
			if(line_pos>11)line_pos=0;// 超过12月（索引11）则重置为1月（索引0）
			if(month>12)month=1;// 超过12月则重置为1月
			key_data.rdata = 0;// 清除按键状态（避免重复处理）
		}
		// 处理确认键（exdata=1表示确认键被按下：返回主菜单）
		if(key_data.exdata == 1)
		{
			//buzzer_buzz(2500, 100);
			vTaskResume(xShowMenuTaskHandle);
			vTaskSuspend(NULL);
			key_data.exdata = 0;
		}
		//u8g2_SendBuffer(&u8g2);
	}
}
