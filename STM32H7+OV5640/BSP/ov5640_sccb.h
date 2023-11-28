#ifndef __OV5640_SCCB_H
#define __OV5640_SCCB_H

#include "main.h"


/* ���Ŷ��� */
#define SCCB_SCL_GPIO_PORT           GPIOB
#define SCCB_SCL_GPIO_PIN            GPIO_PIN_4
#define SCCB_SCL_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)
#define SCCB_SDA_GPIO_PORT           GPIOB
#define SCCB_SDA_GPIO_PIN            GPIO_PIN_3
#define SCCB_SDA_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

/* IO���� */
#define SCCB_SCL(x)                  do{ x ?                                                                                             \
                                                    HAL_GPIO_WritePin(SCCB_SCL_GPIO_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_SET) :  \
                                                    HAL_GPIO_WritePin(SCCB_SCL_GPIO_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                                                }while(0)
#define SCCB_SDA(x)                  do{ x ?                                                                                             \
                                                    HAL_GPIO_WritePin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_SET) :  \
                                                    HAL_GPIO_WritePin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                                                }while(0)
#define SCCB_READ_SDA()              HAL_GPIO_ReadPin(SCCB_SDA_GPIO_PORT, SCCB_SDA_GPIO_PIN)

/* �������� */
void sccb_init(void);
void sccb_3_phase_write(uint8_t id_addr, uint16_t sub_addr, uint8_t dat);
void sccb_2_phase_write(uint8_t id_addr, uint16_t sub_addr);
void sccb_2_phase_read(uint8_t id_addr, uint8_t *dat);

#endif
