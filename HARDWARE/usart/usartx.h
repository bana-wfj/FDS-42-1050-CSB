#ifndef __USARTx_H
#define __USARTx_H

#include <stdint.h>
#include "cs32f0xx_usart.h"

#define XP2116_USART_RX_LEN   20



void xp2116_usart_send_func(uint8_t *data, uint16_t len);

#endif
