/**
 ****************************************************************************************************
 * @file        sdram.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-09-06
 * @brief       SDRAM ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������ H743������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20220906
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/SDRAM/sdram.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/sys/sys.h"
#include "./BSP/LCD/lcd.h"


SDRAM_HandleTypeDef g_sdram_handle;                                           /* SRAM��� */

/**
 * @brief       ��ʼ��SDRAM
 * @param       ��
 * @retval      ��
 */
void sdram_init(void)
{
    FMC_SDRAM_TimingTypeDef sdram_timing;

    g_sdram_handle.Instance = FMC_SDRAM_DEVICE;                               /* SDRAM��BANK5,6 */
    g_sdram_handle.Init.SDBank = FMC_SDRAM_BANK1;                             /* ��һ��SDRAM BANK */
    g_sdram_handle.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;       /* ������ */
    g_sdram_handle.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;            /* ������ */
    g_sdram_handle.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;         /* ���ݿ��Ϊ16λ */
    g_sdram_handle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;    /* һ��4��BANK */
    g_sdram_handle.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;                 /* CASΪ2 */
    g_sdram_handle.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE; /* ʧ��д���� */
    g_sdram_handle.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;             /* SDRAMʱ��=pll2_r_ck/2=220M/2=110M=9.1ns */
    g_sdram_handle.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;                  /* ʹ��ͻ�� */
    g_sdram_handle.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;              /* ��ͨ����ʱ */
    
    sdram_timing.LoadToActiveDelay = 2;                                       /* ����ģʽ�Ĵ���������ʱ����ӳ�Ϊ2��ʱ������ */
    sdram_timing.ExitSelfRefreshDelay = 8;                                    /* �˳���ˢ���ӳ�Ϊ8��ʱ������ */
    sdram_timing.SelfRefreshTime = 7;                                         /* ��ˢ��ʱ��Ϊ7��ʱ������ */
    sdram_timing.RowCycleDelay = 7;                                           /* ��ѭ���ӳ�Ϊ7��ʱ������ */
    sdram_timing.WriteRecoveryTime = 2;                                       /* �ָ��ӳ�Ϊ2��ʱ������ */
    sdram_timing.RPDelay = 2;                                                 /* ��Ԥ����ӳ�Ϊ2��ʱ������ */
    sdram_timing.RCDDelay = 2;                                                /* �е����ӳ�Ϊ2��ʱ������ */
    HAL_SDRAM_Init(&g_sdram_handle, &sdram_timing);

    sdram_initialization_sequence();                                          /* ����SDRAM��ʼ������ */

    /**
     * ˢ��Ƶ�ʼ�����(��SDCLKƵ�ʼ���),���㷽��:
     * COUNT=SDRAMˢ������/����-20=SDRAMˢ������(us)*SDCLKƵ��(Mhz)/����
     * ����ʹ�õ�SDRAMˢ������Ϊ64ms,SDCLK=220/2=110Mhz,����Ϊ8192(2^13).
     * ����,COUNT=64*1000*110/8192-20=839
     */
    HAL_SDRAM_ProgramRefreshRate(&g_sdram_handle, 839);                       /* ����ˢ��Ƶ�� */
}

/**
 * @brief       ����SDRAM��ʼ������
 * @param       ��
 * @retval      ��
 */
void sdram_initialization_sequence(void)
{
    uint32_t temp = 0;

    /* SDRAM��������ʼ������Ժ���Ҫ��������˳���ʼ��SDRAM */
    sdram_send_cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);                /* ʱ������ʹ�� */
    delay_us(500);                                                    /* ������ʱ500us */
    sdram_send_cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);                      /* �����д洢��Ԥ��� */
    sdram_send_cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0);          /* ������ˢ�´��� */

    /* ����ģʽ�Ĵ���,SDRAM��bit0~bit2Ϊָ��ͻ�����ʵĳ��ȣ�
     * bit3Ϊָ��ͻ�����ʵ����ͣ�bit4~bit6ΪCASֵ��bit7��bit8Ϊ����ģʽ
     * bit9Ϊָ����дͻ��ģʽ��bit10��bit11λ����λ */
    temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1  |                  /* ����ͻ������:1(������1/2/4/8) */
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL  |                  /* ����ͻ������:����(����������/����) */
              SDRAM_MODEREG_CAS_LATENCY_2          |                  /* ����CASֵ:2(������2/3) */
              SDRAM_MODEREG_OPERATING_MODE_STANDARD|                  /* ���ò���ģʽ:0,��׼ģʽ */
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;                   /* ����ͻ��дģʽ:1,������� */
    sdram_send_cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp);              /* ����SDRAM��ģʽ�Ĵ��� */
}

/**
 * @brief       SDRAM�ײ��������������ã�ʱ��ʹ��
 * @note        �˺����ᱻHAL_SDRAM_Init()����
 * @param       hsdram   : SDRAM���
 * @retval      ��
 */
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
    GPIO_InitTypeDef gpio_init_struct;

    __HAL_RCC_FMC_CLK_ENABLE();                  /* ʹ��FMCʱ�� */
    __HAL_RCC_GPIOC_CLK_ENABLE();                /* ʹ��GPIOCʱ�� */
    __HAL_RCC_GPIOD_CLK_ENABLE();                /* ʹ��GPIODʱ�� */
    __HAL_RCC_GPIOE_CLK_ENABLE();                /* ʹ��GPIOEʱ�� */
    __HAL_RCC_GPIOF_CLK_ENABLE();                /* ʹ��GPIOFʱ�� */
    __HAL_RCC_GPIOG_CLK_ENABLE();                /* ʹ��GPIOGʱ�� */
    
    gpio_init_struct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3;  
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;     /* ���츴�� */
    gpio_init_struct.Pull = GPIO_PULLUP;         /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;    /* ���� */
    gpio_init_struct.Alternate = GPIO_AF12_FMC;  /* ����ΪFMC */
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);     /* ��ʼ��PC0,2,3 */
    
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &gpio_init_struct);     /* ��ʼ��PD0,1,8,9,10,14,15 */
    
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);     /* ��ʼ��PE0,1,7,8,9,10,11,12,13,14,15 */
    
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOF, &gpio_init_struct);     /* ��ʼ��PF0,1,2,3,4,5,11,12,13,14,15 */
    
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOG, &gpio_init_struct);     /* ��ʼ��PG0,1,2,4,5,8,15 */
}

/**
 * @brief       ��SDRAM��������
 * @param       bankx   : 0,��BANK5�����SDRAM����ָ��
 *   @arg                 1,��BANK6�����SDRAM����ָ��
 * @param       cmd     : ָ��(0,����ģʽ/1,ʱ������ʹ��/2,Ԥ������д洢��/3,�Զ�ˢ��/4,����ģʽ�Ĵ���/5,��ˢ��/6,����)
 * @param       refresh : ��ˢ�´���
 * @retval      ����ֵ:0,����;1,ʧ��.
 * @retval      ģʽ�Ĵ����Ķ���
 */
uint8_t sdram_send_cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval)
{
    uint32_t target_bank = 0;
    FMC_SDRAM_CommandTypeDef command;
    
    if (bankx == 0)
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
    }
    else if (bankx == 1)
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
    }

    command.CommandMode = cmd;                                              /* ���� */
    command.CommandTarget = target_bank;                                    /* Ŀ��SDRAM�洢���� */
    command.AutoRefreshNumber = refresh;                                    /* ��ˢ�´��� */
    command.ModeRegisterDefinition = regval;                                /* Ҫд��ģʽ�Ĵ�����ֵ */

    if (HAL_SDRAM_SendCommand(&g_sdram_handle, &command, 0X1000) == HAL_OK) /* ��SDRAM�������� */
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief       ��ָ����ַ(WriteAddr+BANK5_SDRAM_ADDR)��ʼ,����д��n���ֽ�
 * @param       pbuf      : �ֽ�ָ��
 * @param       writeaddr : Ҫд��ĵ�ַ
 * @param       n         : Ҫд����ֽ���
 * @retval      ��
*/
void fmc_sdram_write_buffer(uint8_t *pbuf, uint32_t writeaddr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *(volatile uint8_t *)(BANK5_SDRAM_ADDR + writeaddr) = *pbuf;
        writeaddr++;
        pbuf++;
    }
}

/**
 * @brief       ��ָ����ַ((WriteAddr+BANK5_SDRAM_ADDR))��ʼ,��������n���ֽ�.
 * @param       pbuf     : �ֽ�ָ��
 * @param       readaddr : Ҫ��������ʼ��ַ
 * @param       n        : Ҫд����ֽ���
 * @retval      ��
*/
void fmc_sdram_read_buffer(uint8_t *pbuf, uint32_t readaddr, uint32_t n)
{
    for (; n != 0; n--)
    {
        *pbuf++ = *(volatile uint8_t *)(BANK5_SDRAM_ADDR + readaddr);
        readaddr++;
    }
}
