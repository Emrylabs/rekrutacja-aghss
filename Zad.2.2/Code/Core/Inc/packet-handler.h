/*
 * packet-handler.h
 *
 *  Created on: 19 paź 2026
 *      Author: Eryk Bujak
 */

#ifndef CORE_INC_PACKET_HANDLER_H_
#define CORE_INC_PACKET_HANDLER_H_

/*
 * Includes
 * */
#include "msg.h"
#include "driver-tlv.h"
#include <string.h>
#include "stm32g4xx_hal.h"

/*
 * Variables
 * */
extern uint16_t pong_delay;

/*
 * Functions
 * */
void Packet_Handler(packet_t req);
void Send_Packet(uint8_t msg_id, uint8_t *data, uint8_t length);
void Send_ACK(message_id_t msg_id, ack_status_t status);
void Send_PING(uint16_t pong_delay_ms);
void Send_PONG();
void Send_MATH(char operation, int16_t a, int16_t b);
void Send_MATH_RESP(int16_t result, uint8_t overflow, uint8_t divide_by_zero);

#endif
