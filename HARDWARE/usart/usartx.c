#include "usartx.h"
#include "xp2116.h"

/**
* 初始化 USART 的 GPIO 口
*/
static void usart_gpio_init(void)
{
	//是能 GPIOA 时钟
	__RCU_AHB_CLK_ENABLE(RCU_AHB_PERI_GPIOA);
	
	//设置 GPIO2、3、9、10 的功能为多功能复用
	GPIOA->PFR &= ~((3 << (2 * 2)) | (3 << (3 * 2)) | (3 << (9 * 2)) | (3 << (10 * 2)));
	GPIOA->PFR |= ((2 << (2 * 2)) | (2 << (3 * 2)) | (2 << (9 * 2)) | (2 << (10 * 2)));
	
	//上拉
	GPIOA->PUPDR &= ~((3 << (2 * 2)) | (3 << (3 * 2)) | (3 << (9 * 2)) | (3 << (10 * 2)));
	GPIOA->PUPDR |= ((1 << (2 * 2)) | (1 << (3 * 2)) | (1 << (9 * 2)) | (1 << (10 * 2)));
	
	
	//复用为 usart
	GPIOA->MFSEL[0] &= ~((0xF << (2 * 4)) | (0xF << (3 * 4)));  //GPIOA 2、3
	GPIOA->MFSEL[1] &= ~((0xF << (1 * 4)) | (0xF << (2 * 4))); //GPIOA 9、10
	
	GPIOA->MFSEL[0] |= ((1 << (2 * 4)) | (1 << (3 * 4)));  //GPIOA 2、3
	GPIOA->MFSEL[1] |= ((1 << (1 * 4)) | (1 << (2 * 4)));  //GPIOA 2、3
}


/*************************************************************** xp2116 USART ************************************************************************************/

/**
*@brief 初始化 timer6 ，用于 xp2116 接收超时计时。
*
*/
static void timer6_init(uint16_t psa, uint16_t arr)
{
	tim_base_t ptr_base;
	//开启 TIMER6 时钟
	__RCU_APB1_CLK_ENABLE(RCU_APB1_PERI_TIM6);
	
	ptr_base.clk_division = TIM_CKDIV_DIV1;
	ptr_base.count_mode = TIM_COUNT_PATTERN_UP;
	ptr_base.period = arr;
	ptr_base.predivider = psa;
	tim_base_init(TIM6, &ptr_base);
	
	//清除标志位
	__TIM_FLAG_CLEAR(TIM6, TIM_FLAG_UPDATE);
	
	//开启timer6中断标志位
	__TIM_INTR_ENABLE(TIM6, TIM_INTR_UPDATE);
	
		//设置中断
	__NVIC_SetPriority(IRQn_USART2, 1);
	__NVIC_EnableIRQ(IRQn_USART2);
	
	__TIM_DISABLE(TIM6);
}


/**
*@brief 初始化 xp2116 的串口
*@param baud: 波特率
*/
void xp2116_usart_init(uint16_t baud)
{
	
	usart_config_t ptr_config;
	//开启 USART2 时钟
	__RCU_APB1_CLK_ENABLE(RCU_APB1_PERI_USART2);
	
	//初始化 IO
	usart_gpio_init();
	
	//定时 10 ms
	timer6_init(2400-1, 100 - 1);
	
	ptr_config.baud_rate = baud;
	ptr_config.data_width = USART_DATA_WIDTH_8;
	ptr_config.flow_control = USART_FLOW_CONTROL_NONE;
	ptr_config.parity = USART_PARITY_NO;
	ptr_config.stop_bits = USART_STOP_BIT_1;
	ptr_config.usart_mode = USART_MODE_TX | USART_MODE_RX;
	usart_init(USART2, &ptr_config);
	
	
	//开启接收中断
	__USART_INTR_ENABLE(USART2, RXNE);
	
	//设置中断
	__NVIC_SetPriority(IRQn_USART2, 1);
	__NVIC_EnableIRQ(IRQn_USART2);
}


/**
*@brief usart2 实际发送数据函数
*@param data: 需要发送的数据
*@param len: 发送数据的长度
*/
void xp2116_usart_send_func(uint8_t *data, uint16_t len)
{
	uint16_t i = 0;
	uint16_t timeout = 0;
	
	for(i = 0; i < len; i++) {
		
		while(!(USART2->STS & (1 << 6))) {
			timeout++;
			if(timeout >= 1000)
				break;
		}
		USART2->TXBUF = data[i];	
		timeout = 0;		
	}
	
	
}


static void timer6_restart(void)
{
	__TIM_DISABLE(TIM6);
	TIM6->CNT = 0;
	__TIM_ENABLE(TIM6);
}

static void timer6_stop(void)
{
	__TIM_DISABLE(TIM6);
	TIM6->CNT = 0;
}


uint8_t XP2116_USART_RX_COUNT = 0;
uint8_t XP2116_USART_RX_BUF[XP2116_USART_RX_LEN];


/**
*
*/
void USART2_IRQHandler(void)
{
	//接收中断
	if(__USART_INTR_STATUS_GET(USART2, RXNE)) {
		
			//重新计数
			timer6_restart();
			
			if(XP2116_USART_RX_COUNT >= XP2116_USART_RX_LEN)
				XP2116_USART_RX_COUNT = 0;
			
			XP2116_USART_RX_BUF[XP2116_USART_RX_COUNT++] = USART2->RXBUF;			
	}
	
}



/**
*
*/
void TIM6_IRQHandler(void) 
{
	if(__TIM_INTR_STATUS_GET(TIM6, UPDATE)) {
		
		timer6_stop();	
		
		if(XP2116_USART_RX_COUNT != 0)	{
			xp2116_rx_complete(XP2116_USART_RX_BUF, XP2116_USART_RX_COUNT);	
			XP2116_USART_RX_COUNT = 0;
		}
	}
}

/*********************************************************************** xp2116 USART end *********************************************************************************/