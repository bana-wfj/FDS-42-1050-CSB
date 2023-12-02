#include "main.h"


static void system_lock_config(void);

int main(void)
{
	system_lock_config();
	
	
	while(1) {
		
		
	}
}




/**
*@brief 配置系统时钟为 24MHz
*/
static void system_lock_config(void)
{
	//Reset all clock config
	rcu_def_init();

	//open HRC clock
	RCU->CTR |= (1 << 0);
	
	//Waits for HXT stabilization
	rcu_hxt_stabilization_wait();
	
	//HTX / 2
	RCU->CFG2 = RCU_PLL_CFG_HXT_DIV2;
	//INPUT * 3
	RCU->CFG |= RCU_PLL_MULTI_3;
	//set PLL input clock is HXT
	RCU->CFG |= (2 << 15);
	
	//open PLL clock
	RCU->CTR |= (1 << 24);
	//Waits for PLL stabilization
	while(!((RCU->CTR) & (1 << 25)));
	
	//sysclk input clock is PLL
	RCU->CFG |= (2 << 0);
		
}