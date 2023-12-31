/*
 * camera_run.c
 *
 *  Created on: Nov 17, 2023
 *      Author: smile
 */

#include "camera_run.h"
#include "ov5640.h"
#include "ov5640_cfg.h"
#include "ov5640_sccb.h"
#include "ov5640_dcmi.h"
#include "delay.h"
#include "stdio.h"
#include "TFTLCD.h"
#include "string.h"
#include "user_uart.h"

extern DCMI_HandleTypeDef hdcmi;
extern DMA_HandleTypeDef hdma_dcmi;


#define RGB565_OUTPUT_HEIGHT   480
#define RGB565_OUTPUT_WIDTH    640

#define JPEG_OUTPUT_WIDTH  320
#define JPEG_OUTPUT_HEIGHT 240

/* 定义JPEG图像缓存空间大小 */
#define JPEG_BUF_SIZE  (70*1024)

/* JPEG图像缓存空间 */
volatile uint32_t g_jpeg_buf[JPEG_BUF_SIZE / sizeof(uint32_t)];

uint16_t framebuffer[RGB565_OUTPUT_HEIGHT*RGB565_OUTPUT_WIDTH] __attribute__((section(".sram_data2")));//(0xC0000000 + 2 * (1280 * 800 * 2))


camera_t cam;

uint8_t *p_jpeg_buf;
uint32_t jpeg_len;
uint32_t jpeg_index;
uint32_t jpeg_start_index;
uint32_t jpeg_end_index;

void camera_RGB565_init(void)
{
	uint8_t ret;
    ret  = ov5640_init();                                               /* 初始化OV5640模块 */
    ret += ov5640_set_output_format(OV5640_OUTPUT_FORMAT_RGB565);   /* 设置OV5640输出RGB565图像数据 */
    ret += ov5640_auto_focus_init();                                    /* 初始化OV5640模块自动对焦 */
//    ret += ov5640_auto_focus_continuance();                             /* OV5640模块持续自动对焦 */
//    ret += ov5640_set_light_mode(OV5640_LIGHT_MODE_ADVANCED_AWB);   /* 设置OV5640模块灯光模式 */
//    ret += ov5640_set_color_saturation(OV5640_COLOR_SATURATION_4);  /* 设置OV5640模块色彩饱度 */
//    ret += ov5640_set_brightness(OV5640_BRIGHTNESS_4);              /* 设置OV5640模块亮度 */
//    ret += ov5640_set_contrast(OV5640_CONTRAST_4);                  /* 设置OV5640模块对比度 */
//    ret += ov5640_set_hue(OV5640_HUE_6);                            /* 设置OV5640模块色相 */
//    ret += ov5640_set_special_effect(OV5640_SPECIAL_EFFECT_NORMAL); /* 设置OV5640模块特殊效果 */
//    ret += ov5640_set_exposure_level(OV5640_EXPOSURE_LEVEL_5);      /* 设置OV5640模块曝光度 */
//    ret += ov5640_set_sharpness_level(OV5640_SHARPNESS_OFF);        /* 设置OV5640模块锐度 */
//    ret += ov5640_set_mirror_flip(OV5640_MIRROR_FLIP_1);            /* 设置OV5640模块镜像/翻转 */
//    ret += ov5640_set_test_pattern(OV5640_TEST_PATTERN_OFF);        /* 设置OV5640模块测试图案 */
    ret += ov5640_set_output_size(RGB565_OUTPUT_WIDTH, RGB565_OUTPUT_HEIGHT);         /* 设置OV5640模块输出图像尺寸 */
    if (ret != 0)
    {
        printf("OV5640 init failed!\r\n");
        while (1)
        {

        }
    }

}

void camera_JPEG_init(void)
{
	uint8_t ret;
    ret  = ov5640_init();                                               /* 初始化OV5640模块 */
    ret += ov5640_set_output_format(OV5640_OUTPUT_FORMAT_JPEG);   /* 设置OV5640输出JPEG图像数据 */
    ret += ov5640_auto_focus_init();                                    /* 初始化OV5640模块自动对焦 */
    ret += ov5640_auto_focus_continuance();                             /* OV5640模块持续自动对焦 */
    ret += ov5640_set_light_mode(OV5640_LIGHT_MODE_ADVANCED_AWB);   /* 设置OV5640模块灯光模式 */
    ret += ov5640_set_color_saturation(OV5640_COLOR_SATURATION_4);  /* 设置OV5640模块色彩饱度 */
    ret += ov5640_set_brightness(OV5640_BRIGHTNESS_4);              /* 设置OV5640模块亮度 */
    ret += ov5640_set_contrast(OV5640_CONTRAST_4);                  /* 设置OV5640模块对比度 */
    ret += ov5640_set_hue(OV5640_HUE_6);                            /* 设置OV5640模块色相 */
    ret += ov5640_set_special_effect(OV5640_SPECIAL_EFFECT_NORMAL); /* 设置OV5640模块特殊效果 */
    ret += ov5640_set_exposure_level(OV5640_EXPOSURE_LEVEL_5);      /* 设置OV5640模块曝光度 */
    ret += ov5640_set_sharpness_level(OV5640_SHARPNESS_OFF);        /* 设置OV5640模块锐度 */
    ret += ov5640_set_mirror_flip(OV5640_MIRROR_FLIP_1);            /* 设置OV5640模块镜像/翻转 */
    ret += ov5640_set_test_pattern(OV5640_TEST_PATTERN_OFF);        /* 设置OV5640模块测试图案 */
    ret += ov5640_set_pre_scaling_window(4, 0);
    ret += ov5640_set_output_size(JPEG_OUTPUT_WIDTH, JPEG_OUTPUT_HEIGHT);         /* 设置OV5640模块输出图像尺寸 */
    if (ret != 0)
    {
        printf("OV5640 init failed!\r\n");
        while (1)
        {

        }
    }

}


/**
 * @brief      DCMI接口帧中断回调函数
 * @param
 * @retval
 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
//    HAL_DCMI_Suspend(hdcmi); // 拍照完成，挂起 DCMI
//    HAL_DCMI_Stop(hdcmi);    // 拍照完成，停止 DMA传输
	cam.frame_sta = 1;  //帧中断完成标志置位
}

//void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
//{
//    HAL_DCMI_Suspend(hdcmi); // 拍照完成，挂起 DCMI
//    HAL_DCMI_Stop(hdcmi);    // 拍照完成，停止 DMA传输
//	cam.frame_sta = 1;  //帧中断完成标志置位
//}

//void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
//{
//
//}

/**
 * @brief       开始OV5640模块DCMI接口DMA传输
 * @param       dts_addr        : 帧数据的接收目的地址
 *              len             : 传输的帧数据大小
 * @retval      无
 */
void ov5640_dcmi_start(ov5640_output_format_t mode,uint32_t dts_addr,uint32_t len)
{
	/* 清空帧接收完成标记
	 * 使能DCMI帧接收中断
	 */
	cam.frame_sta = 0;
	if(mode==OV5640_OUTPUT_FORMAT_RGB565)
	{
		hdcmi.DMA_Handle->Init.MemInc = DMA_MINC_ENABLE;
		hdcmi.DMA_Handle->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	    __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);
		HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, dts_addr, len);
	}
	else if(mode==OV5640_OUTPUT_FORMAT_JPEG)
	{
		hdcmi.DMA_Handle->Init.MemInc = DMA_MINC_ENABLE;
		hdcmi.DMA_Handle->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		__HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);
//        HAL_DMAEx_MultiBufferStart(&hdcmi.DMA_Handle,(uint32_t)&hdcmi.DMA_Handle->DR,(uint32_t)&dcmi_line_buf[0],(uint32_t)&dcmi_line_buf[1],jpeg_line_size);//开启双缓冲
//		__HAL_DMA_ENABLE(hdcmi.DMA_Handle); //使能DMA
//		__HAL_DMA_ENABLE_IT(hdcmi.DMA_Handle,DMA_IT_TC);    //开启传输完成中断

		HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)dts_addr, len);
	}

	/* 等待传输完成 */
	while (cam.frame_sta == 0);
	HAL_DCMI_Stop(&hdcmi);
}


/**
 * @brief       例程演示入口函数
 * @param       无
 * @retval      无
 */
void TFT_LCD_RGB565_run(void)
{

    uint16_t height_index;
    uint16_t width_index;
	/* 获取图像数据,数据存储在framebuffer中*/
    memset((void *)framebuffer, 0, RGB565_OUTPUT_HEIGHT*RGB565_OUTPUT_WIDTH*2);
    ov5640_dcmi_start(OV5640_OUTPUT_FORMAT_RGB565,(uint32_t)&framebuffer,RGB565_OUTPUT_WIDTH*RGB565_OUTPUT_HEIGHT/2 );
	/* 显示至LCD */
    //使用DMAD区域填充显示，LTDC时钟频率50M时屏幕会闪烁，改为37.5M解决，原因应该时显示屏幕的连接排线太长导致
    dma2d_fill_rect(framebuffer,(uint32_t *)LCD_FRAME_BUFFER,RGB565_OUTPUT_WIDTH,RGB565_OUTPUT_HEIGHT,0,1024-RGB565_OUTPUT_WIDTH);

    //使用画点的方式显示，刷新频率较慢
    //	for (height_index=0; height_index<RGB565_OUTPUT_HEIGHT; height_index++)
//	{
//		for (width_index=0; width_index<RGB565_OUTPUT_WIDTH; width_index++)
//		{
//			//lcd_draw_point(width_index, height_index, framebuffer[height_index * RGB565_OUTPUT_WIDTH + width_index]);
//		}
//	}

}



void JPEG_run(void) //目前测试图像会有断层，未找到解决办法
{
	p_jpeg_buf = (uint8_t *)g_jpeg_buf;
    jpeg_len = JPEG_BUF_SIZE / (sizeof(uint32_t));
    memset((void *)g_jpeg_buf, 0, JPEG_BUF_SIZE);

   ov5640_dcmi_start(OV5640_OUTPUT_FORMAT_JPEG,(uint32_t)g_jpeg_buf,g_out_width*g_out_height/sizeof(uint32_t) );
  /* 获取JPEG图像数据起始位置 */
  jpeg_start_index = 0xffffffff;
  do
  {
	  for (jpeg_index=0; jpeg_index<JPEG_BUF_SIZE - 1; jpeg_index++)
	  {
		   if ((p_jpeg_buf[jpeg_index] == 0xFF) && (p_jpeg_buf[jpeg_index + 1] == 0xD8)&& (p_jpeg_buf[jpeg_index + 2] == 0xFF))
		   {
			   HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
			   jpeg_start_index = jpeg_index;
			   break;
		   }
	  }
  }while(jpeg_start_index==0xffffffff);


//   /* 获取JPEG图像数据结束位置 */
  jpeg_end_index = 0xffffffff;
  do
  {
	  for (jpeg_index=0; jpeg_index<JPEG_BUF_SIZE - 1; jpeg_index++)
	  {
		   if ((p_jpeg_buf[jpeg_index] == 0xFF) && (p_jpeg_buf[jpeg_index + 1] == 0xD9))
		   {
			   jpeg_end_index = jpeg_index;
			   HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
			   break;
		   }
	  }
  }while(jpeg_end_index==0xffffffff);


  //   /* 获取JPEG图像数据的长度 */
     jpeg_len = jpeg_end_index - jpeg_start_index + (sizeof(uint32_t) >> 1);

  //   /* 发送JPEG图像数据 */
     if((jpeg_len>2)&&(jpeg_len<JPEG_BUF_SIZE))
     {
         for (jpeg_index=jpeg_start_index; jpeg_index<jpeg_len; jpeg_index++)
         {
      	   log_com_uart_send_byte(p_jpeg_buf[jpeg_index]);
         }
     }

}
