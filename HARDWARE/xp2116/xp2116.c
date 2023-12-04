/*************************************************************
Copyright: Unless otherwise stated, ownership is vested in Eaglerise.
File name: xp2116 module
Description:  control xp2116 peripherals to complete basic dimming.
Author: bana.wfj
Version: rev1.0.0
Date: 2023-11-28
*************************************************************/

#include "xp2116.h"
#include "cs32f0xx.h"
#include "usartx.h"
#include <string.h>

//定义 xp2116 的数据发送队列和接收队列
static bool first_init = true;
static pingbuffer_t xp2116_tx_queue;
static pingbuffer_t xp2116_rx_queue;


//指向 xp2116 设备实体指针
static xp2116_t *XP2116_DEVICE_BUF[XP2116_MALLOC_DEVICE_NUM];

//存储最新读取命令的寄存器地址
static uint8_t NEWEST_READ_ADDR = 0;


/**
*@brief 软件延时函数
*@param time: 延时的时间
*/
static void delay_us(uint16_t time)
{
	uint8_t i = 0;
	for(; time > 0; time--)
		for(i = 0; i < 24; i++){
			
		}
}


/**
*@brief xp2116 私有调光数据封装函数
*@param id: xp2116 对应的 ID
*@param data: 对应的数据，如果是普通模式，该数据对应地址的顺序是：0x22、0x23、0x21; 专家模式的顺序是: 0x22、0x21
*@param 
*/
static void xp2116_dimmer_set_cmd(uint8_t id, uint8_t *data, uint8_t len)
{
	uint8_t i = 0;
	uint8_t data_len = 0;
	uint8_t len_id_wr = 0;
	uint8_t temp_buf[20];
	

//	len_id_wr = (1 - 1) << 4; //length
	len_id_wr |= REG_WRITE << 3; //R/W
	len_id_wr |= (id & 0x7);  //ID
	
	for(i = 0; i < len; i++) {
		// 装载数据
		temp_buf[data_len++] = XP2116_CMD_HAND;
		temp_buf[data_len++] = len_id_wr;
		
		switch (i) {
		  case 0:
			temp_buf[data_len++] = XP2116_SET_DIMMER_VALUE_M_REG;
			break;
		  case 1:
			if(len == 2) 
				temp_buf[data_len++] = XP2116_SET_DIMMER_VALUE_L_REG;
			else if(len == 3)
				temp_buf[data_len++] = XP2116_SET_DIMMER_VALUE_H_REG;
			break;
		  case 2:
			temp_buf[data_len++] = XP2116_SET_DIMMER_VALUE_L_REG;
			break;
		}
		
		temp_buf[data_len++] = data[i];
	}
	
	pingbuffer_enter_queue(&xp2116_tx_queue, temp_buf, data_len);	
}



/**
*@brief 数据封装
*@param id_num: xp2116 对应的 ID 值
*@param read_write: 0-read, 1-write
*@param reg: 读写寄存器地址
*@param data: 读写的数据
*@param len：读写寄存器的长度
*/
static void xp2116_device_set_cmd(uint8_t id, uint8_t read_write, uint8_t reg,	\
								   uint8_t *data, uint8_t len)
{
	uint8_t i=0;
	uint8_t data_len = 0;
    uint8_t len_id_wr = 0;
    
    uint8_t temp_buf[20];
	
	len_id_wr = (len - 1) << 4; //length
	len_id_wr |= read_write << 3; //R/W
	len_id_wr |= (id & 0x7);  //ID
	
	// 装载数据
	temp_buf[data_len++] = XP2116_CMD_HAND;
	temp_buf[data_len++] = len_id_wr;
	temp_buf[data_len++] = reg;
	
	
	if(read_write) {
		for(i = 0; i < len; i++) {		
			temp_buf[data_len++] = data[i];
		}
	}
	
	pingbuffer_enter_queue(&xp2116_tx_queue, temp_buf, data_len);
}



/**
*@brief 更新 xp2116 的状态，实际是读取 xp2116 寄存器的值
*@param xp2116: 需要更新的 xp2116 实体
*/
static void xp2116_update_state(xp2116_t *xp2116)
{
	if(xp2116->state >= XP2116_STATE_INITIALIZING) {
		//读取读写寄存器的值
		xp2116_device_set_cmd(xp2116->init.id, REG_READ, XP2116_SET_WORK_PATTERN_REG, NULL, XP2116_RDWR_REG_LENGHT);
		//读取只读寄存器的值
		xp2116_device_set_cmd(xp2116->init.id, REG_READ, XP2116_OUTPUT_CURRENT_L_REG, NULL, XP2116_READONLY_REG_LENGHT);
	}
}


/**
*@brief 封装 xp2116 的发送函数 
*@param 发送数据
*@param 发送的长度
*/
static void xp2116_usart_send_data(uint8_t *data, uint16_t len)
{
	xp2116_usart_send_func(data, len);
}

/**
*@brief xp2116 发送数据函数，是所有 xp2116 共用的发送函数
*/
static void xp2116_send_func(void)
{
	uint8_t xp2116_id;
	item_t data_item;
	
	//取出一项数据
	if(pingbuffer_exit_queue(&xp2116_tx_queue, &data_item) != 0)
		return;
	
	//取出 xp2116 的 ID
	xp2116_id = (data_item.data[TX_BUF_CMD_INDEX] & 0x07);
	
	//xp2116 ID 不符合要求则直接退出
	if((xp2116_id == 0) || (xp2116_id > 7))
		return;
	
	//ID 的转换，因为 xp2116 的 ID 是从 1 开始的，转换为 0 开始，对应数组下标。
	xp2116_id -= 1;
	
	//判断 xp2116 有效了才会去发送命令
	if(XP2116_DEVICE_BUF[xp2116_id]->state >= XP2116_STATE_INITIALIZING)  {
	
		//调光命令
		if(data_item.data[TX_BUF_REG_ADDR_INDEX] == XP2116_SET_DIMMER_VALUE_M_REG) {
			
			//专家模式
			if(data_item.len >= 8) {
				xp2116_usart_send_data(&data_item.data[0], 4);
				delay_us(50);
				xp2116_usart_send_data(&data_item.data[4], 4);
				
				//普通模式
				if(data_item.len == 12) {
					delay_us(50);
					xp2116_usart_send_data(&data_item.data[8], 4);
				}   
			}
			
		} else {   //其他命令
			
			if((data_item.data[TX_BUF_CMD_INDEX] & (1 << 3)) == 0) {     //读命令
				
				NEWEST_READ_ADDR = data_item.data[TX_BUF_REG_ADDR_INDEX];	
			}
			//发送数据
			xp2116_usart_send_data(data_item.data, data_item.len);
		}
	}
}



/**
*@brief 处理接收数据函数
*/
static void xp2116_rx_data_proces_func(void)
{
	uint8_t i = 0;
	uint8_t xp2116_id;
	item_t data_item;
	
	//取出一项数据
	if(pingbuffer_exit_queue(&xp2116_rx_queue, &data_item) != 0) {
		return;
	}
	
	//通过 ID 判断具体的 xp2116 实体
	xp2116_id = (data_item.data[RX_BUF_CMD_INDEX] & 0x07);
	
	//xp2116 ID 不符合要求则直接退出
	if((xp2116_id == 0) || (xp2116_id > 7))
		return;
	
	//ID 的转换，因为 xp2116 的 ID 是从 1 开始的，转换为 0 开始，对应数组下标。
	xp2116_id -= 1;
	
	//属于读写寄存器
	if(SELECT_REG_RANGE(data_item.data[RX_BUF_REG_ADDR_INDEX]) == WRITE_READ_REG) {
		
		//成功读取 xp2116 状态寄存器，则表示该 xp2116 可用。
		if((XP2116_DEVICE_BUF[xp2116_id]->state == XP2116_STATE_INITIALIZING) 
		   && (data_item.data[RX_BUF_REG_ADDR_INDEX] == XP2116_SET_WORK_PATTERN_REG)) {
			   
			   XP2116_DEVICE_BUF[xp2116_id]->state = XP2116_STATE_USABLE;
		}
		
		//将寄存器的值写入定义的数组中。
		for(i = 0; i < (data_item.len - RX_BUF_DATA_START_INDEX); i++) {
			
			if((data_item.data[RX_BUF_REG_ADDR_INDEX] + i) >  XP2116_SET_CURRENT_REGULATIONG_REG)
				break;
					
			XP2116_DEVICE_BUF[xp2116_id]->rdwr_reg[(data_item.data[RX_BUF_REG_ADDR_INDEX] - XP2116_SET_WORK_PATTERN_REG) + i]    \
				= data_item.data[RX_BUF_DATA_START_INDEX + i];
		}
		
		
	} else if(SELECT_REG_RANGE(data_item.data[RX_BUF_REG_ADDR_INDEX]) == READ_ONLY_REG) { //属于只读寄存器
		
				//将寄存器的值写入定义的数组中。
		for(i = 0; i < (data_item.len - RX_BUF_DATA_START_INDEX); i++) {
			
			if((data_item.data[RX_BUF_REG_ADDR_INDEX] + i) >  XP2116_PROTECT_TIME_H_REG)
				break;
					
			XP2116_DEVICE_BUF[xp2116_id]->only_read_reg[(data_item.data[RX_BUF_REG_ADDR_INDEX] - XP2116_OUTPUT_CURRENT_L_REG) + i]    \
				= data_item.data[RX_BUF_DATA_START_INDEX + i];
		}	
	}
}


/**
*@brief 初始化 xp2116 组件
*@param xp2116: xp2116 实体
*/
void xp2116_init(xp2116_t *xp2116)
{
	//第一次初始化,
	if(first_init == true) {
		//初始化发送和接收队列
		pingbuffer_init(&xp2116_tx_queue, XP2116_TX_QUEUE_LEN);
		pingbuffer_init(&xp2116_rx_queue, XP2116_RX_QUEUE_LEN);	
		first_init = false;
	}
	
	//xp2116 ID 的有效范围
	if((xp2116->init.id >= 1) && (xp2116->init.id <= 7)) {
		
		XP2116_DEVICE_BUF[xp2116->init.id - 1] = xp2116;
		XP2116_DEVICE_BUF[xp2116->init.id - 1]->state = XP2116_STATE_INITIALIZING;	
		//
		xp2116_update_state(xp2116);
	}
}


/**
*@brief xp2116 的调光函数
*@param xp2116 指向 xp2116 的实体
*@param value: 调光值，最大为 0xFFFF
*/
void xp2116_dimmer_func(xp2116_t *xp2116, uint16_t value)
{
	
	uint8_t len = 0;
	uint8_t data[3]; 
	uint16_t light_value = 0;
	//判断该 xp2116 是否可用
	if(xp2116->state == XP2116_STATE_USABLE) {		
		
		//普通模式
		if(xp2116->init.mode == NORMAL_MODE) {
			len = 3;
			//将 16 位数据转换成 11 位
			light_value = (value * 0x7FF) / 0xFFFF;
			//0x22 地址数据
			data[0] = (light_value >> 8) | 0xF8;
			//0x23 地址数据
			data[1] = 0x3F;
			//0x21 地址数据
			data[2] = light_value & 0xFF;
			
		} else if(xp2116->init.mode == EXPERT_MODE) { //专家模式
			len = 2;			
			//0x22 地址数据
			data[0] = (value >> 8);
			//0x21 地址数据
			data[1] = value & 0xFF;
		}
		//数据封装与入队
		xp2116_dimmer_set_cmd(xp2116->init.id, data, len);
	}
}


/**
*@brief xp2116 开机函数
*@param xp2116: xp2116 实体
*@param mode: xp2116 工作模式
*/
void xp2116_poweron(xp2116_t *xp2116, xp2116_mode_e mode)
{
	uint8_t bro_in, bro_out;
	uint8_t brown_reg = 0;
	uint8_t old_mode;
	
	bro_out = ((((10 * xp2116->init.brown_out) / 4) - 50) / 10);
	bro_in = ((((10 * xp2116->init.brown_in) / 4) - 65) / 10);
	
	brown_reg = ((bro_out << 4) & 0xF0) | (bro_in & 0x0F);
	
	//设置的 brown in 和 brown out 的电压与 xp2116 寄存器值不一致，则更新它
	if(brown_reg != xp2116->rdwr_reg[XP2116_SET_BROWN_IN_OUT_REG - XP2116_RDWR_REG_START_ADDR]) {
		
		xp2116_device_set_cmd(xp2116->init.id, REG_WRITE, XP2116_SET_BROWN_IN_OUT_REG, &brown_reg, 1);
	}
	
	old_mode = xp2116->rdwr_reg[XP2116_SET_WORK_PATTERN_REG - XP2116_RDWR_REG_START_ADDR] & 0x03;
	//设置模式与当前模式不一致，则需更新
	if(mode != old_mode) {
		old_mode = (uint8_t)mode;		
		xp2116_device_set_cmd(xp2116->init.id, REG_WRITE, XP2116_SET_WORK_PATTERN_REG, &old_mode, 1);
	}
}

/**
*@brief xp2116 关机函数
*@param xp2116: xp2116 实体
*/
void xp2116_poweroff(xp2116_t *xp2116)
{
	uint8_t mode = 0;
	
	xp2116->init.mode = POWEROFF;
	//
	xp2116_dimmer_func(xp2116, 0);
	//关机
	xp2116_device_set_cmd(xp2116->init.id, REG_WRITE, XP2116_SET_WORK_PATTERN_REG, &mode, 1);
}


/**
*@brief xp2116 需要定时执行的函数，最好 1ms 执行一次
*/	
void xp2116_run_time(void)
{
	static uint32_t run_time = 0;  //记录运行时间
	static bool if_need_update = false;
	static uint8_t device_num = 0;
	static uint8_t time_inter = 0;
	
	//判断发送队列是否为空
	if(!is_empty_queue(&xp2116_tx_queue)) {		
		xp2116_send_func();	
	}
	
	//判断接收队列是否为空
	if(!is_empty_queue(&xp2116_rx_queue)) {		
		xp2116_rx_data_proces_func();
	}
	
	if((run_time % XP2116_REG_UPDATE_TIME) == 0) {
		if_need_update = true;
	}
	
	//需要更新状态，每个 xp2116 的更新间隔时间为 5ms
	if(if_need_update) {	
		if(time_inter > XP2116_UPDATE_INTERVAL) {
			if(device_num >= XP2116_MALLOC_DEVICE_NUM) {	//全部扫描完成了
				device_num = 0;
				if_need_update = false;  
			}
			//找到有效的 xp2116 进行更新
			if(XP2116_DEVICE_BUF[device_num]->state >= XP2116_STATE_INITIALIZING) {				
				xp2116_update_state(XP2116_DEVICE_BUF[device_num]);
			}
			
			device_num++;
			time_inter = 0;
		} else {
			time_inter++;
		}
	}
	
	run_time++;
}


/**
*@brief 读取 xp2116 对应寄存器的值
*@param xp2116: xp2116 实体
*@param reg_addr: 寄存器地址
*@param data: 保存读取到的数据
*@param len: 读取的长度
*@return 返回实际读取的长度
*/
uint8_t get_reg_value(xp2116_t *xp2116, uint8_t reg_addr, uint8_t *data, uint8_t len)
{
	uint8_t i = 0;
	
	if(xp2116->state != XP2116_STATE_USABLE)
		return 0;
	
	
	//读写寄存器
	if(SELECT_REG_RANGE(reg_addr) == WRITE_READ_REG) {
		
		for(i = 0; i < len; i++) {
			
			if(((reg_addr - XP2116_RDWR_REG_START_ADDR) + i) >= XP2116_RDWR_REG_LENGHT)
				break;
			
			data[i] = xp2116->rdwr_reg[(reg_addr - XP2116_RDWR_REG_START_ADDR) + i];
		}
		
		
	} else if(SELECT_REG_RANGE(reg_addr) == READ_ONLY_REG) {   //只读寄存器
		
		for(i = 0; i < len; i++) {
			
			if(((reg_addr - XP2116_ONLYREAD_REG_START_ADDR) + i) >= XP2116_READONLY_REG_LENGHT)
				break;
			
			data[i] = xp2116->only_read_reg[(reg_addr - XP2116_ONLYREAD_REG_START_ADDR) + i];
		}
		
	}
	
	return i;
}


/**
*@brief xp2116 接收数据完成，并将数据放入队列。
*@param data: 接收到的数据
*@param len: 接收到数据的长度
*注意: 不能在该函数中做太多事情，因为该函数在中断调用。
*/
void xp2116_rx_complete(uint8_t *data, uint8_t len)
{
	uint8_t i = 0;
	uint8_t data_temp[ITEM_MAX_LENGHT];
	
	data_temp[0] = NEWEST_READ_ADDR;
	
	for(i = 0; i < len; i++) {
		data_temp[i + 1] = data[i];
	}
	
	pingbuffer_enter_queue(&xp2116_rx_queue, data_temp, len + 1);
}