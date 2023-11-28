/*
 * TFTLCD.c
 *
 *  Created on: Sep 21, 2023
 *      Author: smile
 */

#include "TFTLCD.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
//#include "user_delay.h"
#include "font.h"

extern DMA2D_HandleTypeDef hdma2d;



//DMA2D传输数据函数
static void dma2d_transfer_data_r2m(uint32_t *addr, uint32_t xSize, uint32_t ySize, uint32_t offsetLine, uint16_t color)
{
    DMA2D->CR = DMA2D_R2M;   // dma2d mode: register to memory.
    DMA2D->OPFCCR = DMA2D_OUTPUT_RGB565;

    DMA2D->OCOLR = color;
    DMA2D->OMAR = (uint32_t)addr;
    DMA2D->OOR = offsetLine;
    DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize;

    DMA2D->CR |= DMA2D_CR_START;
    while (DMA2D->CR & DMA2D_CR_START);
}

static void dma2d_memcopy(void * psrc, int width, int height, int offLinesrc, int offLinedst)
{
	DMA2D->CR &= ~(DMA2D_CR_START);                           /* 先停止DMA2D */
    DMA2D->CR      = DMA2D_M2M;
    DMA2D->FGMAR   = (uint32_t)psrc;
    DMA2D->OMAR    = (uint32_t)LCD_FRAME_BUFFER;
    DMA2D->FGOR    = offLinesrc;
    DMA2D->OOR     = offLinedst;
    DMA2D->FGPFCCR = DMA2D_OUTPUT_RGB565;
    DMA2D->NLR     = (uint32_t)(width << 16) | (uint16_t)height;

    DMA2D->CR   |= DMA2D_CR_START;
    while (DMA2D->CR & DMA2D_CR_START);
    DMA2D->IFCR |= DMA2D_FLAG_TC;                             /* 清除传输完成标志 */
}


//DMA2D区域填充
void dma2d_fill_rect(void * psrc, void * pdts,int width, int height, int offLinesrc, int offLinedst)
{
	dma2d_memcopy( (void*)psrc, width, height, offLinesrc, offLinedst);
}



//清屏函数
void TFTLCD_clear(uint16_t color)
{
#if USE_DMA2D_EN
    dma2d_transfer_data_r2m((uint32_t *)LCD_FRAME_BUFFER, LCD_WIDTH, LCD_HEIGHT, 0, color);
#else
    uint16_t *ptr = (uint16_t*)LCD_FRAME_BUFFER;
    uint32_t i = 0;

    while (i++ < LCD_WIDTH*LCD_HEIGHT) {
        *(ptr+i) = color;
    }
#endif /* USE_DMA2D_EN */
}

//画点函数
/**
 * @brief	带颜色画点函数
 * @param   x,y	—— 画点坐标 ,color 颜色
 * @return  none
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    uint32_t pos;
    uint16_t *ptr;

    // check position.
    if (x > LCD_WIDTH || y > LCD_HEIGHT) {
        return;
    }

    // calculate the position offset in framebuffer.
    pos = x + y*LCD_WIDTH;
    ptr = (uint16_t*)LCD_FRAME_BUFFER;

    // modify the framebuffer.
#if USE_DMA2D_EN
    dma2d_transfer_data_r2m((uint32_t *)(ptr+pos), 1, 1, 0, color);
#else
    *(ptr+pos) = color;
#endif /* USE_DMA2D_EN */
}

//画线函数
/**
 * @brief	带颜色画线函数
 * @param   x0,y0 起点坐标 ,x1,y1 终点坐标 ,color 颜色
 * @return  none
 */
void lcd_draw_line(uint16_t X0 ,uint16_t Y0,uint16_t X1,uint16_t Y1,uint16_t color)
{
    uint16_t X=0,Y=0;
    int16_t dx=0,dy=0;

    int16_t sub=0;
    int16_t num=0;
    uint16_t temp=0;
    if((X0==X1)&&(Y0==Y1))
    {
            return ;
    }
    dx=abs(X1-X0);
    dy=abs(Y1-Y0);
    if(dx>=dy)//斜率小于等于1
    {
            if(X0>X1) //互换
            {
                    temp=X1;
                    X1=X0;
                    X0=temp;
                    temp=Y1;
                    Y1=Y0;
                    Y0=temp;
            }
            dx=abs(X1-X0);
            dy=abs(Y1-Y0);
            num=(Y1-Y0)>0?1:-1;//判断加减
            sub=(dy<<1)-dx;//sub=dy*2-dx;
            lcd_draw_point(X0,Y0,color);//起点
            for(X=X0,Y=Y0;X<=X1;X++)
            {
                    if(sub>0)
                    {
                            Y+=num;
                            sub+=(dy<<1)-(dx<<1);//sub=2*dy-2*dx
                    }
                    else
                    {
                            sub+=dy<<1;
                    }
                    lcd_draw_point(X,Y,color);//打点
            }
    }
    else //斜率大于1
    {
            if(Y0>Y1)  //互换
            {
                    temp=X1;
                    X1=X0;
                    X0=temp;
                    temp=Y1;
                    Y1=Y0;
                    Y0=temp;
            }
            dx=abs(X1-X0);
            dy=abs(Y1-Y0);
            num=(X1-X0)>0?1:-1;//判断加减
            sub=(dx<<1)-dy;//sub=dy*2-dx;
            lcd_draw_point(X0,Y0,color);//起点
            for(X=X0,Y=Y0;Y<=Y1;Y++)
            {
                    if(sub>0)
                    {
                            X+=num;
                            sub+=(dx<<1)-(dy<<1);//sub=2*dy-2*dx
                    }
                    else
                    {
                            sub+=dx<<1;
                    }
                    lcd_draw_point(X,Y,color);//打点

            }
    }
}


//画圆函数
/**
 * @brief
 * @param   x,y 圆心坐标 ,r 半径 ,color 颜色
 * @return  none
 */
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r,uint16_t color)
{
	uint16_t xm = 0,ym = r;
//	uint16_t i = 0;
	  //先画出圆在x、y轴上的四个点
	  lcd_draw_point(x,y+r,color);
	  lcd_draw_point(x+r,y,color);
	  lcd_draw_point(x,y-r,color);
	  lcd_draw_point(x-r,y,color);

	  while(xm<=ym)//判断是否画完
	  {
	    if((xm*xm+ym*ym)< (r*r))//在圆内
	      xm++;
	    else   //在圆外
	      ym--;
	    lcd_draw_point(x+xm,y+ym,color);
	    lcd_draw_point(x-xm,y+ym,color);
	    lcd_draw_point(x-ym,y+xm,color);
	    lcd_draw_point(x-ym,y-xm,color);
	    lcd_draw_point(x+ym,y+xm,color);
	    lcd_draw_point(x+ym,y-xm,color);
	    lcd_draw_point(x+xm,y-ym,color);
	    lcd_draw_point(x-xm,y-ym,color);
	  }
}

//画实心圆函数
/**
 * @brief
 * @param   x,y 圆心坐标 ,r 半径 ,color 颜色
 * @return  none
 */
void lcd_draw_circle_fill(uint16_t x, uint16_t y, uint16_t r,uint16_t color)
{
	uint16_t i ;
	for(i=1;i<r;i++)
	{
	 lcd_draw_circle(x,y,i,color);
	}
}
/**
 * @breif	带颜色画矩形函数
 * @param   x1,y1 —— 矩形起始点
 * @param	x2,y2 —— 矩形终点
 * @param	color	—— 颜色
 * @retval	none
 */
void LCD_Draw_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	lcd_draw_line(x1,y1,x2,y1,color);
	lcd_draw_line(x1,y1,x1,y2,color);
	lcd_draw_line(x1,y2,x2,y2,color);
	lcd_draw_line(x2,y1,x2,y2,color);
}


/**
 * @breif	读点函数
 * @param   x,y —— 点坐标
 * @retval	color  颜色
 */
uint16_t lcd_read_point(uint16_t x, uint16_t y)
{
    uint16_t *color;

    if (x > LCD_WIDTH || y > LCD_HEIGHT) {
        return 0xffff;
    }

    color = (uint16_t *)((uint32_t)LCD_FRAME_BUFFER+2*y*LCD_WIDTH+2*x);  //*2是因为DMA2D有两个图层

    return *color;

}

//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24/32
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//得到字体一个字符对应点阵集所占的字节数
    num = num - ' '; //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

    for (t = 0; t < csize; t++)
    {
        if (size == 12)temp = asc2_1206[num][t]; 	 	//调用1206字体
        else if (size == 16)temp = asc2_1608[num][t];	//调用1608字体
        else if (size == 24)temp = asc2_2412[num][t];	//调用2412字体
        else if (size == 32)temp = asc2_3216[num][t];	//调用3216字体
        else return;								//没有的字库

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)lcd_draw_point(x, y, STRING_COLOR);
            else if (mode == 0)lcd_draw_point(x, y, WHITE);

            temp <<= 1;
            y++;

            if (y >= 600)return;		//超区域了

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= 1024)return;	//超区域了

                break;
            }
        }
    }
}
//m^n函数
//返回值:m^n次方.
uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}
//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + (size / 2)*t, y, ' ', size, 0);
                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, 0);
    }
}
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)LCD_ShowChar(x + (size / 2)*t, y, '0', size, mode & 0X01);
                else LCD_ShowChar(x + (size / 2)*t, y, ' ', size, mode & 0X01);

                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, mode & 0X01);
    }
}
//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break; //退出

        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}
