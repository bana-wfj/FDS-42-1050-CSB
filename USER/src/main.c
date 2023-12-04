#include "main.h"
#include "delay.h"
#include "usartx.h"

static void system_lock_config(void);


int main(void)
{
	system_lock_config();
	delay_init(SystemCoreClock);
	
	//初始化外设
	xp2116_usart_init(115200);
	
	//运行主任务
	app_main();
	
	while(1) {
		
		
	}
}




/**
*@brief 使用外部晶振，系统频率为 24MHz
*/
static void system_lock_config(void)
{
	//Reset all clock config
	rcu_def_init();

	//open HXT clock
	RCU->CTR |= RCU_CTR_HXTEN;
	
	//wait tiil HXT is ready
    while((RCU->CTR & RCU_CTR_HXTSTAB) == 0)
    {
    }

    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->WCR = FMC_WCR_WE | FMC_WCR_WCNT;
 
    /* HCLK = SYSCLK */
    RCU->CFG |= (uint32_t)RCU_CFG_HCLKPDIV_DIV1;
      
    /* PCLK = HCLK */
    RCU->CFG |= (uint32_t)RCU_CFG_PCLKPDIV_DIV1;

	// HXT / 2
	RCU->CFG2 &= ~0x0000000FUL;
	RCU->CFG2 |= 0x00000001UL;
	
    /* PLL configuration = HXT/2 * 6 = 48 MHz */
    RCU->CFG &= (uint32_t)((uint32_t)~(RCU_CFG_PLLSEL | RCU_CFG_PLLSEL_HXT_PREDIV | RCU_CFG_PLLMUF));
    RCU->CFG |= (uint32_t)(RCU_CFG_PLLSEL_HXT_PREDIV | RCU_CFG_PLLMUF6);
            
    /* Enable PLL */
    RCU->CTR |= RCU_CTR_PLLEN;

    /* Wait till PLL is ready */
    while((RCU->CTR & RCU_CTR_PLLSTAB) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCU->CFG &= (uint32_t)((uint32_t)~(RCU_CFG_SYSSW));
    RCU->CFG |= (uint32_t)RCU_CFG_SYSSW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCU->CFG & (uint32_t)RCU_CFG_SYSSS) != (uint32_t)RCU_CFG_SYSSS_PLL)
    {
    }
		
}

