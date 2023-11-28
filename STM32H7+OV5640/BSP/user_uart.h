/*
 * user_uart.h
 *
 *  Created on: Sep 14, 2023
 *      Author: smile
 */

#ifndef USER_UART_H_
#define USER_UART_H_

#include "main.h"


void log_com_uart_init(void);
void RTK_com_uart_init(void);
void log_com_uart_send_data(uint8_t *buf, uint16_t len);
void log_com_uart_send_byte(uint8_t byte);
#endif /* USER_UART_H_ */
