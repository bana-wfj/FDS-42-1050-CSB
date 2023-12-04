#ifndef __XP2116_H
#define __XP2116_H

#include <stdint.h>
#include "pingbuffer.h"


#define XP2116_CMD_HAND      (0x55)
#define XP2116_CMD_FIXED_LEN  (3)//固定长度

//可以申请的最大 xp2116 的设备数量
#define XP2116_MALLOC_DEVICE_NUM   7
//发送和接收队列的长度
#define XP2116_TX_QUEUE_LEN 		256
#define XP2116_RX_QUEUE_LEN			256

//xp2116 寄存器更新的时间间隔, 100ms
#define XP2116_REG_UPDATE_TIME		100
//每个 xp2116 寄存器更新的间隔, 5ms
#define XP2116_UPDATE_INTERVAL		5

//只读寄存器
#define XP2116_READONLY_REG_LENGHT          14
#define XP2116_PROTECT_TIME_H_REG           0x3D
#define XP2116_PROTECT_TIME_L_REG           0x3C
#define XP2116_OUTPUT_POWER_H_REG           0x3B
#define XP2116_OUTPUT_POWER_L_REG           0x3A
#define XP2116_PEAK_CURRENT_REG             0x39
#define XP2116_AVERAGE_CURRENT_REG          0x38
#define XP2116_PWM_CYCLE_L_REG              0x37
#define XP2116_MAIN_STATE_REG               0x36
#define XP2116_FAULT_TYPE_REG               0x35
#define XP2116_TEMPE_REG                    0x34
#define XP2116_INPUT_VOLTAGE_REG            0x33
#define XP2116_OUTPUT_VOLTAGE_REG           0x32
#define XP2116_OUTPUT_CURRENT_H_REG         0x31
#define XP2116_OUTPUT_CURRENT_L_REG         0x30
#define XP2116_ONLYREAD_REG_START_ADDR		0x30


//读写寄存器
#define XP2116_RDWR_REG_LENGHT                      8                 
#define XP2116_SET_CURRENT_REGULATIONG_REG          0x27
#define XP2116_SET_BROWN_IN_OUT_REG                 0x26
#define XP2116_SET_TEMPE_PROTECT_THRESHOLD_REG      0x24
#define XP2116_SET_DIMMER_VALUE_H_REG               0X23
#define XP2116_SET_DIMMER_VALUE_M_REG               0X22
#define XP2116_SET_DIMMER_VALUE_L_REG               0X21
#define XP2116_SET_WORK_PATTERN_REG                 0x20
#define XP2116_RDWR_REG_START_ADDR					0x20


//发送 buffer 的项目中，数据值对应的下标
#define TX_BUF_FRAME_HEAD_INDEX			0       //帧头在发送 buffer 中对应的下标
#define TX_BUF_CMD_INDEX     			1       //寄存器地址在发送 buffer 中对应的下标
#define TX_BUF_REG_ADDR_INDEX			2		//xp2116 Cmd 在发送 buffer 中对应的下标
#define TX_BUF_DATA_START_INDEX			3       //在发送 buffer 中真正存储第一个数据的下标



//接收 buffer 的项目中，数据值对应的下标
#define RX_BUF_REG_ADDR_INDEX     		0       //寄存器地址在接收 buffer 中对应的下标
#define RX_BUF_CMD_INDEX				1		//xp2116 Cmd 在接收 buffer 中对应的下标
#define RX_BUF_DATA_START_INDEX			2       //在接收 buffer 中真正存储第一个数据的下标




//查找指定寄存器地址属于哪类寄存器
#define SELECT_REG_RANGE(reg_addr) (((reg_addr >= XP2116_SET_WORK_PATTERN_REG) && (reg_addr <= XP2116_SET_CURRENT_REGULATIONG_REG)) ?  \
										WRITE_READ_REG : ((reg_addr >= XP2116_OUTPUT_CURRENT_L_REG) && (reg_addr <= XP2116_PROTECT_TIME_H_REG)) ? READ_ONLY_REG : 0)

//找到该寄存器地址在数组中的下标
#define GET_REG_TO_BUF_INDEX(reg)  (SELECT_REG_RANGE(reg) == WRITE_READ_REG) ？ (reg - XP2116_RDWR_REG_START_ADDR) :  \
										((SELECT_REG_RANGE(reg) == READ_ONLY_REG) ? (reg - XP2116_ONLYREAD_REG_START_ADDR) : 0)


/**
*@enum xp2116_reg_type_e
*@brief xp2116 寄存器类型
*/
typedef enum
{
	WRITE_READ_REG = 1,
	READ_ONLY_REG,
}xp2116_reg_type_e;


typedef enum
{
	REG_READ = 0,
	REG_WRITE,	
}xp2116_rdwr_state_e;


/**
*@enum xp2116_mode_e
*@brief xp2116 工作模式
*/
typedef enum
{
	POWEROFF = 0,
	NORMAL_MODE,			// 普通模式调光
	EXPERT_MODE,      	// 专家模式调光
}xp2116_mode_e;


/**
*@enum xp2116_state_e
*@brief xp2116 设备状态
*/
typedef enum
{
	XP2116_STATE_UNDEFINE = 0,
	XP2116_STATE_CANCELLED,
	XP2116_STATE_INITIALIZING,
	XP2116_STATE_USABLE,
}xp2116_state_e;


/**
*@enum xp2116_fault_reg_e
*@brief xp2116 错误类型
*/
typedef enum
{
	OUTPUT_OVP_PROTECT = 0,
	OUTPUT_OSP_PROTECT,
	INPUT_OVP_PROTECT,
	INPUT_UVP_PROTECT,
	OTP_PROTECT,
	CS_PROTECT,
	UART_POWEROFF,
}xp2116_fault_reg_e;


/**
*@struct xp2116_init_item_t
*@brief xp2116 初始化必须设置初始值的项
*/
typedef struct {
	uint8_t id;
	uint8_t brown_in;
	uint8_t brown_out;
	xp2116_mode_e mode;    //工作模式
}xp2116_init_item_t;


/**
*@struct xp2116_t
*@brief xp2116 组件的结构体
*/
typedef struct 
{
	//xp2116 初始化必须设置初始值的项
	xp2116_init_item_t init;
	
	//以下的项不需要再初始化的时候给出
	xp2116_state_e state;
	xp2116_fault_reg_e fault_reg;
	uint8_t only_read_reg[XP2116_READONLY_REG_LENGHT];
	uint8_t rdwr_reg[XP2116_RDWR_REG_LENGHT];
}xp2116_t;


/**************************** 函数定义区 ********************************/

void xp2116_init(xp2116_t *xp2116);
void xp2116_dimmer_func(xp2116_t *xp2116, uint16_t value);
void xp2116_poweron(xp2116_t *xp2116, xp2116_mode_e mode);
void xp2116_poweroff(xp2116_t *xp2116);
void xp2116_run_time(void);
void xp2116_rx_complete(uint8_t *data, uint8_t len);
uint8_t get_reg_value(xp2116_t *xp2116, uint8_t reg_addr, uint8_t *data, uint8_t len);

#endif

