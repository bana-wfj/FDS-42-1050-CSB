/*
 * c_buffer_stream.h
 *
 *  Created on: 2023-5-8
 *      Author: jianbin.liu
 */

#ifndef C_BUFFER_STREAM_H_
#define C_BUFFER_STREAM_H_

#define SEND_BUFFER_SIZE (50)  // 一共可以缓存多少条命令
#define MAX_LEN_OF_EACH_COMMAND (6) // 一个命令的最大长度

#include <stdint.h>

#include <stdbool.h>

#define NO (0)
#define YES (1)

#define NG (0)
#define OK (1)


typedef struct
{
        uint8_t DataLen; //
        uint8_t Data[MAX_LEN_OF_EACH_COMMAND]; // 4-7 CAN id
} FRAME_INFO;

typedef struct
{
        uint8_t ReadIndex ; // 将要读的地址
        uint8_t WriteIndex ;// 将要写的地址
        uint8_t  ArrayLen; // 队列总SEND_BUFFER_SIZE
        FRAME_INFO Array[SEND_BUFFER_SIZE]; // buf of uart
         bool FullFlag ; // 队列是否已经满了
         bool lock ;
} ST_BUF_MEMORY;



void queueInit(ST_BUF_MEMORY *queue, uint8_t buf_len);
bool EX_Buf_IS_EMPTY(ST_BUF_MEMORY *pCanArray);
void EX_Buf_CleanBuf(ST_BUF_MEMORY *pCanArray);
extern uint8_t EX_Buf_QueueIn(ST_BUF_MEMORY *pCanArray  , FRAME_INFO *pCanData);
extern uint16_t EX_Buf_HasDataInStream(ST_BUF_MEMORY *pCanArray);// For uart
extern uint8_t EX_Buf_QueueOut(ST_BUF_MEMORY *pCanArray  ,  FRAME_INFO *pCanData);

#endif /* C_BUFFER_STREAM_H_ */
