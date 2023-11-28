/*
 * sdram.c
 *
 *  Created on: Nov 2, 2023
 *      Author: smile
 */

#include "sdram.h"

extern SDRAM_HandleTypeDef hsdram1;


#define EXT_SDRAM_ADDR  	((uint32_t)0xC0000000)
#define EXT_SDRAM_SIZE		(32 * 1024 * 1024)






static int SDRAM_SendCommand(uint32_t CommandMode, uint32_t Bank, uint32_t RefreshNum, uint32_t RegVal)
{
    uint32_t CommandTarget;
    FMC_SDRAM_CommandTypeDef Command;

    if (Bank == 1) {
        CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    } else if (Bank == 2) {
        CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
    }

    Command.CommandMode = CommandMode;
    Command.CommandTarget = CommandTarget;
    Command.AutoRefreshNumber = RefreshNum;
    Command.ModeRegisterDefinition = RegVal;

    if (HAL_SDRAM_SendCommand(&hsdram1, &Command, 0x1000) != HAL_OK) {
        return -1;
    }

    return 0;
}



void SDRAM_Init(void)
{
    uint32_t temp;

    /* 1. 时钟使能命令 */
    SDRAM_SendCommand(FMC_SDRAM_CMD_CLK_ENABLE, 1, 1, 0);

    /* 2. 延时，至少100us */
//    HAL_Delay(1);

    /* 3. SDRAM全部预充电命令 */
    SDRAM_SendCommand(FMC_SDRAM_CMD_PALL, 1, 1, 0);

    /* 4. 自动刷新命令 */
    SDRAM_SendCommand(FMC_SDRAM_CMD_AUTOREFRESH_MODE, 1, 8, 0);

    /* 5. 配置SDRAM模式寄存器 */
    temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1            |          //设置突发长度：1
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL     |          //设置突发类型：连续
                     SDRAM_MODEREG_CAS_LATENCY_3             |          //设置CL值：3
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD   |          //设置操作模式：标准
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;              //设置突发写模式：单点访问
    SDRAM_SendCommand(FMC_SDRAM_CMD_LOAD_MODE, 1, 1, temp);

    /* 6. 设置自刷新频率 */
    /*
        SDRAM refresh period / Number of rows）*SDRAM时钟速度 – 20
      = 64000(64 ms) / 4096 *108MHz - 20
      = 1667.5 取值1668
    */
    HAL_SDRAM_ProgramRefreshRate(&hsdram1, 1668);
}


/*********************************************************************************************************
*	函 数 名: bsp_TestExtSDRAM
*	功能说明: 扫描测试外部SDRAM的全部单元。
*	形    参: 无
*	返 回 值: 0 表示测试通过； 大于0表示错误单元的个数。
*********************************************************************************************************
*/
uint32_t bsp_TestExtSDRAM(void)
{
	uint32_t i;
	uint32_t *pSRAM;
	uint8_t *pBytes;
	uint32_t err;
	const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

	/* 写SDRAM */
	pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		*pSRAM++ = i;
	}

	/* 读SDRAM */
	err = 0;
	pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != i)
		{
			err++;
		}
	}

	if (err >  0)
	{
		return  (4 * err);
	}

	/* 对SDRAM 的数据求反并写入 */
	pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		*pSRAM = ~*pSRAM;
		pSRAM++;
	}

	/* 再次比较SDRAM的数据 */
	err = 0;
	pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != (~i))
		{
			err++;
		}
	}

	if (err >  0)
	{
		return (4 * err);
	}

	/* 测试按字节方式访问, 目的是验证 FSMC_NBL0 、 FSMC_NBL1 口线 */
	pBytes = (uint8_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		*pBytes++ = ByteBuf[i];
	}

	/* 比较SDRAM的数据 */
	err = 0;
	pBytes = (uint8_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		if (*pBytes++ != ByteBuf[i])
		{
			err++;
		}
	}
	if (err >  0)
	{
		return err;
	}
	return 0;
}


/* USER CODE END 0 */

