#include "delay.h"


static uint32_t systick_time;


/**
*@brief 初始化 systick 定时器
*/
void delay_init(uint32_t system_clock)
{
	uint32_t clock;
	
	clock = system_clock / 8000;
	// 1ms
	SysTick_Config(clock);
	
}


/**
*@brief systick 中断服务函数
*/
void SysTick_Handler(void)
{
	systick_time++;
}


/**
*@brief 延时函数，最小延时时间为 1ms
*@param 延时时间
*/
void delay_ms(uint32_t time)
{
	uint32_t start_time = systick_time;
	
	while(1) {
		if((systick_time - start_time) >= time)
			break;
	}
}


/**
*@brief 给定一个开始时间，判断是否超时
*@param 给定的开始时间
*@param 延时的时间
*@return true表示已经超时，反之
*/
bool wait_delay_time(uint32_t start_time, uint32_t delay_time)
{
	return ((systick_time - start_time) >= delay_time) ? true : false;
}


/**
*@brief 返回当前时间
*/
uint32_t get_current_time(void)
{
	return systick_time;
}

