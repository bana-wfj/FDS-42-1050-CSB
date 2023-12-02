/*************************************************************************
Copyright: 归属权归 Eaglerise 所有
File name: pingbuffer.c
Description: 使用数据的队列操作
Author: bana.wei
Version: V1.0.0
Data: 2023-11-29
History: 无
*************************************************************************/
#include "pingbuffer.h"

/// <summary>
/// 队列初始化
/// </summary>
/// <param name="queue"> 队列的实体 </param>
/// <param name="len">队列的长度，单位为字节</param>
void pingbuffer_init(pingbuffer_t* queue, uint16_t len)
{
	if (len > QUEUE_MAX_LENGHT) {
		len = QUEUE_MAX_LENGHT;
	}

	queue->len = len;
	queue->full_flg = false;
	queue->rd = 0;
	queue->wr = 0;
}


/// <summary>
/// 判断队列是否为空
/// </summary>
/// <param name="queue"> 队列的实体 </param>
/// <returns> </returns>
bool is_empty_queue(pingbuffer_t* queue)
{
	if (queue->full_flg == true)
		return false;
	return (queue->wr == queue->rd) ? true : false;
}

/// <summary>
/// 计算队列的剩余空间，单位为字节
/// </summary>
/// <param name="queue"> 队列实体 </param>
/// <returns> 返回队列的剩余字节数 </returns>
uint16_t queue_used_space(pingbuffer_t* queue)
{
	return ((uint32_t)(queue->wr - queue->rd) % queue->len);
}


/// <summary>
/// 入队
/// </summary>
/// <param name="queue">队列实体</param>
/// <param name="data">需要入队的数据</param>
/// <param name="len">需要入队的数据的长度</param>
/// <returns></returns>
uint8_t pingbuffer_enter_queue(pingbuffer_t* queue, uint8_t* data, uint16_t len)
{
	uint16_t i = 0;
	//	item_t new_item;
	uint8_t buf[ITEM_MAX_LENGHT] = { 0 };

	if(queue->full_flg == true)
		return QUEUE_FULL;

	//项目的长度不符合规定
	if((len > (ITEM_MAX_LENGHT - ITEM_HEAD_LEN)) || (len == 0))
		return ITEM_LEN_ERROR;
		
	
	
	//new_item.head = (ITEM_HEAD1 << 8) | ITEM_HEAD2;
	//new_item.len = ITEM_HEAD_LEN + len;
	//new_item.data = data;

	buf[0] = ITEM_HEAD1;
	buf[1] = ITEM_HEAD2;
	buf[2] = ITEM_HEAD_LEN + len;

	//要写入的数据长度大于队列的剩余长度
	if ((queue->len - queue_used_space(queue)) < buf[2]) {
		return NO_FREE_SPACE;
	}

	for (i = 0; i < len; i++) {
		buf[ITEM_HEAD_LEN + i] = data[i];
	}

	for (i = 0; i < buf[2]; i++) {
		if (queue->wr >= queue->len) {
			queue->wr = 0;
		}
		queue->queue[queue->wr++] = buf[i];
	}

	//队列已满
	if (queue->wr == queue->rd)
		queue->full_flg = true;

	return SUCCESSFULLY;
}

/// <summary>
/// 出队
/// </summary>
/// <param name="queue">队列实体</param>
/// <param name="data">存储返回的一个项目</param>
/// <returns></returns>
uint8_t pingbuffer_exit_queue(pingbuffer_t* queue, item_t* data)
{
	uint8_t find_flg = false;
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t len = queue_used_space(queue);


	if (is_empty_queue(queue))
		return QUEUE_EMPTY;

	//从队列中寻找一个项目
	for (i = 0; i < len; i++) {
		if (queue->rd >= queue->len)
			queue->rd = 0;

		if (queue->queue[queue->rd] == ITEM_HEAD1) {
			data->head = (ITEM_HEAD1 << 8);
			find_flg++;
		}
		else if ((find_flg == 1) & (queue->queue[queue->rd] == ITEM_HEAD2)) {
			data->head |= ITEM_HEAD2;
			find_flg++;
		}
		else if ((find_flg == 1) & (queue->queue[queue->rd] != ITEM_HEAD2)) {
			find_flg = 0;
		}
		else if (find_flg == 2) {
			data->len = queue->queue[queue->rd] - ITEM_HEAD_LEN;
			find_flg++;
		}
		else if (find_flg == 3) {
			data->data[j++] = queue->queue[queue->rd];
			if (j == data->len) {
				queue->rd++;
				break;
			}		
		}
		queue->rd++;
	}

	queue->full_flg = false;

	//扫描整个队列都没有发现可用的项目
	if ((i == queue_used_space(queue)) && (j != data->len))
		return NO_USEFUL_ITEM;

	return SUCCESSFULLY;
}


