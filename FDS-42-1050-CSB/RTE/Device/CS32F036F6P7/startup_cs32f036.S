;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2017 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD. All rights reserved.                           */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/
     MODULE  ?cstartup


     ;; Forward declaration of sections.
     SECTION CSTACK:DATA:NOROOT(3)
     
     SECTION .intvec:CODE:NOROOT(2)

     EXTERN  __iar_program_start
     EXTERN  SystemInit
     PUBLIC  __vector_table
     
     
   
     
     DATA
__vector_table  
	DCD     sfe(CSTACK)               	   ; Top of Stack        0
	DCD     Reset_Handler                  ; Reset Handler
	DCD     NMI_Handler                    ; NMI Handler
	DCD     HardFault_Handler              ; Hard Fault Handler
	DCD     0                              ; Reserved
	DCD     0                              ; Reserved
	DCD     0                              ; Reserved
	DCD     0                              ; Reserved
	DCD     0                              ; Reserved
	DCD     0                              ; Reserved
	DCD     0                              ; Reserved
	DCD     SVC_Handler                    ; SVCall Handler
	DCD     0                              ; Reserved
	DCD     0                              ; Reserved
	DCD     PendSV_Handler                 ; PendSV Handler
	DCD     SysTick_Handler                ; SysTick Handler

	; External Interrupts handler
	DCD     WWDT_IRQHandler                ; Window Watchdog
	DCD     LVD_IRQHandler                 ; LVD through EXTI Line detect
	DCD     0                 	       ; Reserved
	DCD     FLASH_IRQHandler               ; FLASH
	DCD     RCU_IRQHandler                 ; RCU
	DCD     EXTI0_1_IRQHandler             ; EXTI Line 0 and 1
	DCD     EXTI2_3_IRQHandler             ; EXTI Line 2 and 3
	DCD     EXTI4_15_IRQHandler            ; EXTI Line 4 to 15
	DCD     0                              ; Reserved
	DCD     DMA1_Channel1_IRQHandler       ; DMA1 Channel 1
	DCD     DMA1_Channel2_3_IRQHandler     ; DMA1 Channel 2 and Channel 3
	DCD     DMA1_Channel4_5_IRQHandler     ; DMA1 Channel 4 and Channel 5
	DCD     ADC1_IRQHandler                ; ADC1 
	DCD     TIM1_BRK_UP_TRG_COM_IRQHandler ; TIM1 Break, Update, Trigger and Commutation
	DCD     TIM1_CC_IRQHandler             ; TIM1 Capture Compare
	DCD     0                              ; Reserved
	DCD     TIM3_IRQHandler                ; TIM3
	DCD     TIM6_IRQHandler                ; TIM6
	DCD     0                              ; Reserved
	DCD     TIM14_IRQHandler               ; TIM14
	DCD     TIM15_IRQHandler               ; TIM15
	DCD     TIM16_IRQHandler               ; TIM16
	DCD     TIM17_IRQHandler               ; TIM17
	DCD     I2C1_IRQHandler                ; I2C1
	DCD     I2C2_IRQHandler                ; I2C2
	DCD     SPI1_IRQHandler                ; SPI1
	DCD     SPI2_IRQHandler                ; SPI2
	DCD     USART1_IRQHandler              ; USART1
	DCD     USART2_IRQHandler              ; USART2 
	DCD     0                              ; Reserved
                              
     THUMB
 
     PUBWEAK Reset_Handler
     SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0
        
            
        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)          
NMI_Handler            
                B       NMI_Handler
         
        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)      
HardFault_Handler    
                B      HardFault_Handler
         
        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler         
                B      SVC_Handler
            
        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler      
                B       PendSV_Handler
               
        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)     
SysTick_Handler        
                B       SysTick_Handler
     
        PUBWEAK WWDT_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
WWDT_IRQHandler        
                B      WWDT_IRQHandler
				
        PUBWEAK LVD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
LVD_IRQHandler        
                B      LVD_IRQHandler                
                
        PUBWEAK FLASH_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
FLASH_IRQHandler        
                B       FLASH_IRQHandler
                
        PUBWEAK RCU_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
RCU_IRQHandler        
                B       RCU_IRQHandler
                
        PUBWEAK EXTI0_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
EXTI0_1_IRQHandler        
                B       EXTI0_1_IRQHandler
                
        PUBWEAK EXTI2_3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
EXTI2_3_IRQHandler        
                B       EXTI2_3_IRQHandler
                
        PUBWEAK EXTI4_15_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
EXTI4_15_IRQHandler        
                B       EXTI4_15_IRQHandler
                
        PUBWEAK DMA1_Channel1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
DMA1_Channel1_IRQHandler        
                B       DMA1_Channel1_IRQHandler
                
        PUBWEAK DMA1_Channel2_3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
DMA1_Channel2_3_IRQHandler        
                B       DMA1_Channel2_3_IRQHandler
                
        PUBWEAK DMA1_Channel4_5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
DMA1_Channel4_5_IRQHandler        
                B       DMA1_Channel4_5_IRQHandler
                
        PUBWEAK ADC1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
ADC1_IRQHandler        
                B       ADC1_IRQHandler
                
        PUBWEAK TIM1_BRK_UP_TRG_COM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
TIM1_BRK_UP_TRG_COM_IRQHandler        
                B       TIM1_BRK_UP_TRG_COM_IRQHandler
                
        PUBWEAK TIM1_CC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)     
TIM1_CC_IRQHandler        
                B       TIM1_CC_IRQHandler  
				
       PUBWEAK TIM3_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
TIM3_IRQHandler        
                B      TIM3_IRQHandler  
				
		PUBWEAK TIM6_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
TIM6_IRQHandler        
                B      TIM6_IRQHandler    				
								
		PUBWEAK TIM14_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
TIM14_IRQHandler        
                B      TIM14_IRQHandler 
												
		PUBWEAK TIM15_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
TIM15_IRQHandler        
                B      TIM15_IRQHandler 
															
		PUBWEAK TIM16_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
TIM16_IRQHandler        
                B      TIM16_IRQHandler 

       PUBWEAK TIM17_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
TIM17_IRQHandler        
                B      TIM17_IRQHandler  
				
		PUBWEAK I2C1_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
I2C1_IRQHandler        
                B      I2C1_IRQHandler    				
								
		PUBWEAK I2C2_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
I2C2_IRQHandler        
                B      I2C2_IRQHandler 
												
		PUBWEAK SPI1_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
SPI1_IRQHandler        
                B      SPI1_IRQHandler 
															
		PUBWEAK SPI2_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
SPI2_IRQHandler        
                B      SPI2_IRQHandler
				
		PUBWEAK USART1_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
USART1_IRQHandler        
                B      USART1_IRQHandler 
															
		PUBWEAK USART2_IRQHandler
       SECTION .text:CODE:REORDER:NOROOT(1)     
USART2_IRQHandler        
                B      USART2_IRQHandler		
					
         END