#ifndef __DATA_H__
#define __DATA_H__

#include <stdint.h>
#include "u8g2.h"

//UI结构体
typedef struct UI{
	const char name[20];		// 应用名称（字符串）
	uint8_t num;						// 应用标识ID（整数）
	const uint8_t data[128];// 图标位图数据（XBM格式，变长数组）
	int32_t x;							// 图标绘制X坐标
	int32_t y;							// 图标绘制Y坐标
	int32_t w;							// 图标宽度（像素）
	int32_t h;							// 图标高度（像素）
}ui;

//图像数据结构体
typedef struct Images{
	int x;
	int y;
	int w;
	int h;
}Image;

//字符串链表
typedef struct str1{
	char *string;
	struct str1 *next_str;
}str1, *p_str;//str1=结构体类型，p_str=结构体指针类型


void u8g2_config(void);
void ui_run(int* a, int* a_trg, int b);
void ui_left(int32_t* a, int b);
void ui_right(int32_t* a, int b);
void ui_up(int32_t* a, int b);
void ui_down(int32_t* a, int b);

extern const uint8_t ShowPower[30];
extern const uint8_t ShowGame[30];
extern const uint8_t LeftMove[30];
extern const uint8_t RightMove[30];
extern const uint8_t BigNum[10][120];
extern const uint8_t light[];
extern const uint8_t Num_6x8[12][8];
extern const uint8_t wooden_flsh[2][384];
extern const uint8_t gongde[64]; 
extern const uint8_t hammer[40];

extern ui cleder;
extern ui torch;
extern ui hum;
extern ui clock;
extern ui setting;

#endif
