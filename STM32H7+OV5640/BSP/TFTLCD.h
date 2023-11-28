/*
 * TFTLCD.h
 *
 *  Created on: Sep 21, 2023
 *      Author: smile
 */

#ifndef TFTLCD_H_
#define TFTLCD_H_

#include "main.h"

#define USE_DMA2D_EN        1


#define LCD_WIDTH       1024
#define LCD_HEIGHT      600

#define LCD_FRAME_BUFFER    0xc0000000

/**
 * @brief   color
 * @note    rgb565
*/
#define BLACK   0x0000
#define BLUE    0x001F
#define GREEN   0x07E0
#define GBLUE   0X07FF
#define GRAY    0X8430
#define BROWN   0XBC40
#define RED     0xF800
#define PINK    0XF81F
#define BRRED   0XFC07
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define STRING_COLOR   RED

void TFTLCD_clear(uint16_t color);
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r,uint16_t color);
void lcd_draw_circle_fill(uint16_t x, uint16_t y, uint16_t r,uint16_t color);

void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);						//显示一个字符
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size);  						//显示一个数字
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);				//显示 数字
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);		//显示一个字符串,12/16字体
void dma2d_fill_rect(void * psrc, void * pdts,int width, int height, int offLinesrc, int offLinedst);

#endif /* TFTLCD_H_ */
