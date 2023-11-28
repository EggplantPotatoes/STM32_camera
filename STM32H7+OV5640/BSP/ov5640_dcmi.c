//#include "ov5640_dcmi.h"
//
//extern DCMI_HandleTypeDef hdcmi;
//extern DMA_HandleTypeDef hdma_dcmi;
//
//
//static struct
//{
//    DCMI_HandleTypeDef dcmi;
//    uint8_t frame_sem;
//} g_atk_mc5640_dcmi_sta = {0};
//

///**
// * @brief      DCMI接口帧中断回调函数
// * @param
// * @retval
// */
//void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
//{
//        g_atk_mc5640_dcmi_sta.frame_sem = 1;  //帧中断完成标志置位
//
//}
//
//
///**
// * @brief       开始OV5640模块DCMI接口DMA传输
// * @param       dts_addr        : 帧数据的接收目的地址
// *              len             : 传输的帧数据大小
// * @retval      无
// */
//void atk_mc5640_dcmi_start(uint32_t dts_addr,uint32_t len)
//{
//	    /* 清空帧接收完成标记
//	     * 使能DCMI帧接收中断
//	     */
//	    g_atk_mc5640_dcmi_sta.frame_sem = 0;
//	    __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);
//	    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, dts_addr, len);
//
//	    /* 等待传输完成 */
//	    while (g_atk_mc5640_dcmi_sta.frame_sem == 0);
//	    HAL_DCMI_Stop(&g_atk_mc5640_dcmi_sta.dcmi);
//}
