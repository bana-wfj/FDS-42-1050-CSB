/*
 * c_buffer_stream.c
 *
 *  Created on: 2023-5-6
 *      Author: jianbin.liu
 */



#include "c_buffer_stream.h"

uint32_t testin;
uint32_t testout;
uint8_t err_time;
uint16_t read = 0 , write = 0 ;
#if 1



void queueInit(ST_BUF_MEMORY *queue, uint8_t buf_len)
{
//    gSendBuffer.ReadIndex = 0 ;
//    gSendBuffer.WriteIndex = 0 ;
//    gSendBuffer.FullFlag = 0 ;// 队列未满
//    gSendBuffer.lock = 0 ;
//    gSendBuffer.ArrayLen= SEND_BUFFER_SIZE ; // 队列总SEND_BUFFER_SIZE
    
    queue->ReadIndex = 0;
    queue->WriteIndex = 0;
    queue->FullFlag = 0;
    queue->lock = 0;
    queue->ArrayLen = buf_len;
    
}


void EX_Buf_ReadIndex_Inc(ST_BUF_MEMORY *pCanArray)// For uart
{
    read ++;
    pCanArray->ReadIndex++;
    if(pCanArray->ReadIndex == pCanArray->ArrayLen)
    {
        pCanArray->ReadIndex = 0 ;
    }
}

void EX_Buf_CleanBuf(ST_BUF_MEMORY *pCanArray)
{
    pCanArray->WriteIndex = pCanArray->ReadIndex ;
}


void EX_Buf_WriteIndex_Inc(ST_BUF_MEMORY *pCanArray) // For uart
{
    write++;
    pCanArray->WriteIndex ++ ;
    if(pCanArray->WriteIndex == pCanArray->ArrayLen)
    {
        pCanArray->WriteIndex = 0 ;
    }

    if(pCanArray->WriteIndex == pCanArray->ReadIndex ) // the queue is full
    {
      pCanArray->FullFlag = 1 ;
      // 满了，覆盖旧数据
    //  EX_Buf_ReadIndex_Inc(pCanArray);
      err_time++;
    }
    else
    {
        pCanArray->FullFlag = 0 ;
    }
}

bool EX_Buf_IS_EMPTY(ST_BUF_MEMORY *pCanArray)
{
    return (pCanArray->WriteIndex == pCanArray->ReadIndex) ? 1 : 0;
}

// 返回队列中还有几组数据
uint16_t EX_Buf_HasDataInStream(ST_BUF_MEMORY *pCanArray)// For uart
{
    uint16_t res = 0 ;
    
    if(pCanArray->lock  == true)
    {
      return res;
    }
    
    if(pCanArray->WriteIndex >= pCanArray->ReadIndex )
        {
        res = pCanArray->WriteIndex -pCanArray->ReadIndex ;
        }
    else
        {
        res = pCanArray->ArrayLen - ( pCanArray->ReadIndex -  pCanArray->WriteIndex)  ;
        }

    return res ;
}

uint8_t EX_Buf_IsEndOfStream(ST_BUF_MEMORY *pCanArray)//判断接收完后是否处理完
{
    uint8_t res = NO ;
    if (pCanArray->WriteIndex == pCanArray->ReadIndex)
    {
        res = YES ;
    }
    return res ;
}


// 进入队列
uint8_t EX_Buf_QueueIn(ST_BUF_MEMORY *pCanArray  , FRAME_INFO *pCanData) //
{
        uint8_t i = 0 ,res ;
        uint8_t DataLen = 0 ;
        res = OK ;
        testin++;
        pCanArray->lock  = true;
        DataLen = pCanData->DataLen;
        if ( DataLen > MAX_LEN_OF_EACH_COMMAND )
        {
            res = NG ;            //debug_printf("CanBuf_QueueIn fail!\r\n");
            return res ;
        }

        pCanArray->Array[pCanArray->WriteIndex].DataLen = DataLen ;

        for (i = 0 ;i< DataLen ; i++)
        {
        pCanArray->Array[pCanArray->WriteIndex].Data[i] = pCanData->Data[i] ;
        }
        EX_Buf_WriteIndex_Inc(pCanArray);

        pCanArray->lock  = false;

        return res ;
}


// 出队列
uint8_t EX_Buf_QueueOut(ST_BUF_MEMORY *pCanArray  ,  FRAME_INFO *pCanData)
{
    uint8_t i = 0 ,res ;
    uint8_t DataLen  ;
    res = OK ;

    // 清空返回数据，重新填充
    //memset(pCanData , 0, sizeof(FRAME_INFO));
    if(pCanArray->lock  == true)
    {
        res = NG ;
        return res;
    }


    testout++;
    DataLen =  pCanArray->Array[pCanArray->ReadIndex].DataLen ;
    pCanData->DataLen = DataLen ;
    if ( DataLen > MAX_LEN_OF_EACH_COMMAND)
     {
       res = NG ;//debug_printf("CanBuf_QueueIn fail!\r\n");
       return res ;
     }

    for (i = 0 ;i< DataLen ; i++)
     {
        pCanData->Data[i] = pCanArray->Array[pCanArray->ReadIndex].Data[i] ;
     }

    pCanArray->Array[pCanArray->ReadIndex].DataLen = 0;
    EX_Buf_ReadIndex_Inc(pCanArray);

    //   pCanArray->lock  == false;

    return res ;
}
#endif

//////////////////////////////////////////////////



