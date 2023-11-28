/*
 * user_uart.c
 *
 *  Created on: Sep 14, 2023
 *      Author: smile
 */
#include "user_uart.h"
#include "stdio.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

uint8_t StaMessages[4] = {};
uint8_t uart1_RxBuffer[20];
uint8_t uart2_RxBuffer[20];

/**
 * 函数功能: 重定向c库函数printf到DEBUG_USARTx
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

/**
 * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明：无
 */
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}


int _write(int32_t file, uint8_t *ptr, int32_t len) {
	HAL_UART_Transmit(&huart1, ptr, len, HAL_MAX_DELAY);
  return len;
}

void log_com_uart_init(void)
{

//  HAL_UART_Transmit_IT(&huart1, (uint8_t *)StaMessages, sizeof(StaMessages));
  HAL_UART_Receive_IT(&huart1, (uint8_t *)uart1_RxBuffer, 1); // 调用中断接收函数，接收长度设为1，接收一个字节进一次中断
}

void log_com_uart_send_data(uint8_t *buf, uint16_t len)
{

	HAL_UART_Transmit_IT(&huart1, buf, len);
}

void log_com_uart_send_byte(uint8_t byte)
{

	HAL_UART_Transmit(&huart1, &byte, 1, 0xFFFF);
}


void RTK_com_uart_init(void)
{

  HAL_UART_Transmit_IT(&huart2, (uint8_t *)StaMessages, sizeof(StaMessages));
  HAL_UART_Receive_IT(&huart2, (uint8_t *)uart2_RxBuffer, 1); // 调用中断接收函数，接收长度设为1，接收一个字节进一次中断
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
  if(huart->Instance == USART1)
  {
	  HAL_UART_Transmit(&huart1, (uint8_t *)uart1_RxBuffer, 1, 0xFFFF); // 发送接收到的数据
	  HAL_UART_Receive_IT(&huart1, (uint8_t *)uart1_RxBuffer, 1);       // 再开启接收中断（因为里面中断只会触发一次，因此需要再次开启）
  }
  if(huart->Instance == USART2)
  {
	  HAL_UART_Transmit(&huart2, (uint8_t *)uart2_RxBuffer, 1, 0xFFFF); // 发送接收到的数据
	  HAL_UART_Receive_IT(&huart2, (uint8_t *)uart2_RxBuffer, 1);       // 再开启接收中断（因为里面中断只会触发一次，因此需要再次开启）
  }

}

