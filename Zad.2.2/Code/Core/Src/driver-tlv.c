/*
 * driver-tlv.c
 *
 *  Created on: 19 paź 2026
 *      Author: Eryk Bujak
 */
#include "driver-tlv.h"

static uart_send_cb_t uart_send;

static tlv_state_t current_state = RECV_TYPE;
static uint8_t data_left = 1;
static packet_t recv_packet, empty_packet;

/*
 * Read packets are going to FIFO buffer
 * */
static packet_t tlv_buf[TLV_BUF_SIZE];
static uint8_t head = 0;
static uint8_t tail = 0;
static uint8_t count = 0;

/*
 * There are probably a few different ways to achieve code that is going to be universal
 * What I am doing here is passing a wrapper function for HAL UART send function
 * For reading I am just setting an interrupt every received byte and calling a callback
 *
 * Therefore there is no STM HAL in this file, it is all abstracted in the main.c
 * */

void TLV_Init(uart_send_cb_t tx_func) {
	uart_send = tx_func;
}

uint8_t TLV_Send(packet_t msg) {
	if(msg.payload_length > TLV_MAX_PAYLOAD_SIZE) {
		return -1;
	}

	uint8_t msg_buf[msg.payload_length + 2];

	msg_buf[0] = msg.message_id;
	msg_buf[1] = msg.payload_length;
	memcpy(&msg_buf[2], msg.payload, msg.payload_length);

	uart_send(msg_buf, msg.payload_length + 2);

	return 0;
}

void TLV_RecvCallback(uint8_t uart_recv) {
	switch(current_state) {
		case RECV_TYPE:
			recv_packet.message_id = uart_recv;
			current_state = RECV_LENGTH;
			break;
		case RECV_LENGTH:
			recv_packet.payload_length = uart_recv;
			data_left = uart_recv;
			current_state = RECV_PAYLOAD;
			break;
		case RECV_PAYLOAD:
			recv_packet.payload[recv_packet.payload_length - data_left] = uart_recv;
			data_left--;

			if(data_left == 0) {
				// Finished receiving
				if(count >= TLV_BUF_SIZE) {
					head = (head + 1) % TLV_BUF_SIZE;
					count--;
				}

				tlv_buf[tail] = recv_packet;
				tail = (tail + 1) % TLV_BUF_SIZE;
				count++;

				recv_packet = empty_packet;
				current_state = RECV_TYPE;
			}
			break;
		default:
			current_state = RECV_TYPE;
			break;
	}
}

uint8_t TLV_Available() {
	return count > 0;
}

packet_t TLV_Read() {
	if(count == 0) {
		return empty_packet;
	}

	packet_t packet = tlv_buf[head];
	head = (head + 1) % TLV_BUF_SIZE;
	count--;

	return packet;
}
