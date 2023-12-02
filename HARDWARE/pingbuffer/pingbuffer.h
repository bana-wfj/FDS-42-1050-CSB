#ifndef __PINGBUFFER_H
#define __PINGBUFFER_H
#include <stdint.h>
#include <stdbool.h>

//队列的最大长度，单位：字节
#define QUEUE_MAX_LENGHT	256
#define ITEM_MAX_LENGHT		20

#define ITEM_HEAD1 0xFF
#define ITEM_HEAD2 0xAA
#define ITEM_HEAD_LEN  3

typedef enum
{
	SUCCESSFULLY = 0,
	QUEUE_FULL,
	QUEUE_EMPTY,
	NO_USEFUL_ITEM,
	NO_FREE_SPACE,
	ITEM_LEN_ERROR
}return_type_e;






typedef struct
{
	uint16_t head;
	uint8_t len;
	uint8_t data[ITEM_MAX_LENGHT];
}item_t;


typedef struct
{
	bool full_flg;
	uint8_t queue[QUEUE_MAX_LENGHT];
	uint16_t len;
	uint16_t wr;
	uint16_t rd;
}pingbuffer_t;



void pingbuffer_init(pingbuffer_t* queue, uint16_t len);
bool is_empty_queue(pingbuffer_t* queue);
uint16_t queue_used_space(pingbuffer_t* queue);
uint8_t pingbuffer_enter_queue(pingbuffer_t* queue, uint8_t* data, uint16_t len);
uint8_t pingbuffer_exit_queue(pingbuffer_t* queue, item_t* data);

#endif
