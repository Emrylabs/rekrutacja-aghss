/*
 * packet-handler.c
 *
 *  Created on: 19 paź 2026
 *      Author: Eryk Bujak
 */
#include "packet-handler.h"

void Packet_Handler(packet_t req) {
	switch(req.message_id) {
		case MSG_ACK:
			// No need to do anything
			return;
			break;
		case MSG_PING:
			if(req.payload_length != sizeof(msg_ping_t)) {
				Send_ACK(MSG_PING, MSG_STATUS_ERROR);
				return;
			}

			msg_ping_t *ping = (msg_ping_t *)req.payload;
			pong_delay = ping->pong_delay_ms;

			break;
		case MSG_PONG:
			Send_ACK(MSG_PONG, MSG_STATUS_OK);
			break;
		case MSG_MATH:
			if(req.payload_length < sizeof(msg_math_t)) {
				Send_ACK(MSG_MATH, MSG_STATUS_ERROR);
				return;
			}

			msg_math_t *math = (msg_math_t *)req.payload;

			uint8_t active_operations = math->operation_bits.add +
										math->operation_bits.sub +
										math->operation_bits.mul +
										math->operation_bits.div;

			if((active_operations > 1) | (active_operations == 0)) {
				Send_ACK(MSG_MATH, MSG_STATUS_ERROR);
				return;
			}

			int32_t result = 0;
			uint8_t overflow = 0, divide_by_zero = 0;

			if(math->operation_bits.add) {
				result = (int32_t)math->operand_a + math->operand_b;
			} else if(math->operation_bits.sub) {
				result = (int32_t)math->operand_a - math->operand_b;
			} else if(math->operation_bits.mul) {
				result = (int32_t)math->operand_a * math->operand_b;
			} else if(math->operation_bits.div) {
				if(math->operand_b == 0) {
					divide_by_zero = 1;
				} else {
					result = (int32_t)math->operand_a / math->operand_b;
				}
			}

			if(!divide_by_zero) {
				if((result > 32767) || (result < -32768)) {
					overflow = 1;
					result = 0;
				}
			}

			Send_MATH_RESP((uint16_t)result, overflow, divide_by_zero);
			break;
		case MSG_MATH_RESP:
			Send_ACK(MSG_MATH_RESP, MSG_STATUS_OK);
			break;
		default:
			break;
	}
}

void Send_Packet(uint8_t msg_id, uint8_t *data, uint8_t length) {
	packet_t resp = {
		.message_id = msg_id,
		.payload_length = length
	};

	memcpy(resp.payload, data, length);
	TLV_Send(resp);
}

void Send_ACK(message_id_t msg_id, ack_status_t status) {
	msg_ack_t ack_packet;
	ack_packet.ack_msg_id = msg_id;
	ack_packet.ack_status = status;

	Send_Packet(MSG_ACK, (uint8_t *)&ack_packet, sizeof(msg_ack_t));
}

void Send_PING(uint16_t delay) {
	msg_ping_t ping_packet;
	ping_packet.pong_delay_ms = delay;

	Send_Packet(MSG_PING, (uint8_t *)&ping_packet, sizeof(msg_ping_t));
}

void Send_PONG() {
	msg_pong_t pong_packet;

	Send_Packet(MSG_PONG, (uint8_t *)&pong_packet, sizeof(msg_pong_t));
}

void Send_MATH(char operation, int16_t a, int16_t b) {
	msg_math_t math_packet;
	math_packet.operand_a = a;
	math_packet.operand_b = b;

	switch(operation) {
		case '+':
			math_packet.operation_bits.add = 1;
			break;
		case '-':
			math_packet.operation_bits.sub = 1;
			break;
		case '*':
			math_packet.operation_bits.mul = 1;
			break;
		case '/':
			math_packet.operation_bits.div = 1;
			break;
		default:
			return;
	}

	Send_Packet(MSG_MATH, (uint8_t *)&math_packet, sizeof(msg_math_t));
}

void Send_MATH_RESP(int16_t result, uint8_t overflow, uint8_t divide_by_zero) {
	msg_math_resp_t math_resp_packet;
	math_resp_packet.flags_bits.overflow = overflow;
	math_resp_packet.flags_bits.divide_by_zero = divide_by_zero;
	math_resp_packet.result = result;

	Send_Packet(MSG_MATH_RESP, (uint8_t *)&math_resp_packet, sizeof(msg_math_resp_t));
}
