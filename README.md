# DShanMCU-F103 手表项目

基于韦东山瑞士军刀开发板（DShanMCU-F103）的手表小项目，使用HAL库和FreeRTOS实现。

## 功能说明
- 后续计划实现：时间显示，多级菜单显示，万年历（显示2024年份的日历），模拟手电,温湿度显示，电子闹钟，设置（开关系统声音）

## 开发环境
- 开发板：DShanMCU-F103
- 库：STM32 HAL库
- 操作系统：FreeRTOS
- IDE：MDK-ARM（Keil）

## 如何使用
1. 克隆仓库到本地：`git clone https://github.com/r5ett/fl03-FreeRTOS-watch.git`
2. 用STM32CubeMX打开`.ioc`文件，生成代码
3. 在MDK-ARM中打开工程，编译下载到开发板

## 许可证
遵循GPL-3.0许可证（与你的LICENSE文件对应）
