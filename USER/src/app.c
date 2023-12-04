#include "main.h"
#include "xp2116.h"
#include "delay.h"

//
xp2116_t xp2116_id4;

/**
*@brief 初始化 xp2116 组件
*
*
*/
static void app_xp2116_init(xp2116_t * xp2116, uint8_t id)
{
	xp2116->init.id = id;
	xp2116->init.mode = EXPERT_MODE;
	xp2116->init.brown_in = 52;
	xp2116->init.brown_out = 56;
	xp2116_init(xp2116);
}


/**
*@brief 软件定时器任务，1ms 执行一次
*/
static void timer_task(void)
{
	xp2116_run_time();
	
	
}


/**
*@brief 主任务
*/
void app_main(void)
{
	uint32_t start_time;
	uint32_t run_time = 0;
	app_xp2116_init(&xp2116_id4, 4);
	
	
	start_time = get_current_time();
		
	while(1) {
		
		if(wait_delay_time(start_time, 1)) {
			start_time = get_current_time();
			
			//
			timer_task();
			
			
			run_time++;
		}
	}
	
}

