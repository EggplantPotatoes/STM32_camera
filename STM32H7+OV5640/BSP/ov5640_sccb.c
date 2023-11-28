#include "ov5640_sccb.h"
#include "delay.h"


#define SCCB_WRITE   0x00
#define SCCB_READ    0x01


static inline void sccb_delay(void)
{
    delay_us(5);
}


static void sccb_start(void)
{
    SCCB_SDA(1);
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SDA(0);
    sccb_delay();
    SCCB_SCL(0);
}


static void sccb_stop(void)
{
    SCCB_SDA(0);
    sccb_delay();
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SDA(1);
    sccb_delay();
}

/**
 * @brief       SCCB接口主机向从机写入一个字节数据
 * @param       dat: 待写入的一个字节数据
 * @retval      无
 */
static void sccb_write_byte(uint8_t dat)
{
    int8_t dat_index;
    uint8_t dat_bit;
    
    for (dat_index=7; dat_index>=0; dat_index--)
    {
        dat_bit = (dat >> dat_index) & 0x01;
        SCCB_SDA(dat_bit);
        sccb_delay();
        SCCB_SCL(1);
        sccb_delay();
        SCCB_SCL(0);
    }
    
    SCCB_SDA(1);
    sccb_delay();
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SCL(0);
}

/**
 * @brief       SCCB接口主机向从机读取一个字节数据
 * @param       dat: 读取到的一个字节数据
 * @retval      无
 */
static void sccb_read_byte(uint8_t *dat)
{
    int8_t dat_index;
    uint8_t dat_bit;

    for (dat_index=7; dat_index>=0; dat_index--)
    {
        sccb_delay();
        SCCB_SCL(1);
        dat_bit = SCCB_READ_SDA();
        *dat |= (dat_bit << dat_index);
        sccb_delay();
        SCCB_SCL(0);
    }
    
    sccb_delay();
    SCCB_SCL(1);
    sccb_delay();
    SCCB_SCL(0);
    sccb_delay();
    SCCB_SDA(0);
    sccb_delay();
    
}

/**
 * @brief       OV5640 SCCB接口初始化
 * @param       无
 * @retval      无
 */
void sccb_init(void)
{
    sccb_stop();
}

/**
 * @brief       SCCB接口3阶段写传输
 * @param       id_addr : ID Address
 *              sub_addr: Sub-address
 *              dat     : Write Data
 * @retval      无
 */
void sccb_3_phase_write(uint8_t id_addr, uint16_t sub_addr, uint8_t dat)
{
    sccb_start();
    sccb_write_byte((id_addr << 1) | SCCB_WRITE);
    sccb_write_byte((uint8_t)(sub_addr >> 8) & 0xFF);
    sccb_write_byte((uint8_t)sub_addr & 0xFF);
    sccb_write_byte(dat);
    sccb_stop();
}

/**
 * @brief       SCCB接口2阶段写传输
 * @param       id_addr : ID Address
 *              sub_addr: Sub-address
 * @retval      无
 */
void sccb_2_phase_write(uint8_t id_addr, uint16_t sub_addr)
{
    sccb_start();
    sccb_write_byte((id_addr << 1) | SCCB_WRITE);
    sccb_write_byte((uint8_t)(sub_addr >> 8) & 0xFF);
    sccb_write_byte((uint8_t)sub_addr & 0xFF);
    sccb_stop();
}

/**
 * @brief       SCCB接口2阶段读传输
 * @param       id_addr: ID Address
 *              dat: 读取到的数据
 * @retval      无
 */
void sccb_2_phase_read(uint8_t id_addr, uint8_t *dat)
{
    sccb_start();
    sccb_write_byte((id_addr << 1) |SCCB_READ);
    sccb_read_byte(dat);
    sccb_stop();
}
