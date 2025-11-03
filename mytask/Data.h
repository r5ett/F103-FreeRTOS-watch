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


#endif
