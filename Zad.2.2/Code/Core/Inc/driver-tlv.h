/*
 * driver-tlv.h
 *
 *  Created on: 19 paź 2026
 *      Author: Eryk Bujak
 */

#ifndef TLV_DRIVER_H
#define TLV_DRIVER_H

#include "msg.h"
#include <string.h>

#define TLV_BUF_SIZE 8

/*
 * Typedef
 * */
typedef void (*uart_send_cb_t)(uint8_t *data, uint8_t size);

typedef enum {
	RECV_TYPE,
	RECV_LENGTH,
	RECV_PAYLOAD
} tlv_state_t;

/*
 * Functions
 * */
void     TLV_Init(uart_send_cb_t tx_func);
uint8_t  TLV_Send(packet_t msg);
void	 TLV_RecvCallback(uint8_t uart_recv);
uint8_t  TLV_Available();
packet_t TLV_Read();

#endif
