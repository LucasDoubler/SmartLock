/**
 ****************************************************************************************************
 * @file        mpu.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-09-06
 * @brief       MPU ��������
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
 
#include "./BSP/MPU/mpu.h"
#include "./BSP/LED/led.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
 
 
 /**
 * @brief       ����ĳ�������MPU����
 * @param       baseaddr:MPU��������Ļ�ַ(�׵�ַ)
 *              size:MPU��������Ĵ�С(������32�ı���,��λΪ�ֽ�),�����õ�ֵ�ο�:CORTEX_MPU_Region_Size
 *              rnum:MPU���������,��Χ:0~7,���֧��8����������,�����õ�ֵ�ο���CORTEX_MPU_Region_Number
 *              ap:����Ȩ��,���ʹ�ϵ����:�����õ�ֵ�ο���CORTEX_MPU_Region_Permission_Attributes
 *              MPU_REGION_NO_ACCESS,�޷��ʣ���Ȩ&�û������ɷ��ʣ�
 *              MPU_REGION_PRIV_RW,��֧����Ȩ��д����
 *              MPU_REGION_PRIV_RW_URO,��ֹ�û�д���ʣ���Ȩ�ɶ�д���ʣ�
 *              MPU_REGION_FULL_ACCESS,ȫ���ʣ���Ȩ&�û����ɷ��ʣ�
 *              MPU_REGION_PRIV_RO,��֧����Ȩ������
 *              MPU_REGION_PRIV_RO_URO,ֻ������Ȩ&�û���������д��
 *              ���:STM32F7����ֲ�.pdf,4.6��,Table 89.
 *              sen:�Ƿ�������;MPU_ACCESS_NOT_SHAREABLE,������;MPU_ACCESS_SHAREABLE,����
 *              cen:�Ƿ�����cache;MPU_ACCESS_NOT_CACHEABLE,������;MPU_ACCESS_CACHEABLE,����
 *              ben:�Ƿ�������;MPU_ACCESS_NOT_BUFFERABLE,������;MPU_ACCESS_BUFFERABLE,����
 * @retval      0,�ɹ�.
 *              ����,����.
 */
uint8_t mpu_set_protection(uint32_t baseaddr, uint32_t size, uint32_t rnum, uint8_t ap, uint8_t sen, uint8_t cen, uint8_t ben)
{
    MPU_Region_InitTypeDef mpu_initure;

    HAL_MPU_Disable();                                        /* ����MPU֮ǰ�ȹر�MPU,��������Ժ���ʹ��MPU */

    mpu_initure.Enable = MPU_REGION_ENABLE;                   /* ʹ�ܸñ������� */
    mpu_initure.Number = rnum;                                /* ���ñ������� */
    mpu_initure.BaseAddress = baseaddr;                       /* ���û�ַ */
    mpu_initure.Size = size;                                  /* ���ñ��������С */
    mpu_initure.SubRegionDisable = 0X00;                      /* ��ֹ������ */
    mpu_initure.TypeExtField = MPU_TEX_LEVEL0;                /* ����������չ��Ϊlevel0 */
    mpu_initure.AccessPermission = (uint8_t)ap;               /* ���÷���Ȩ��, */
    mpu_initure.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;  /* ����ָ�����(�����ȡָ��) */
    mpu_initure.IsShareable = sen;                            /* �Ƿ���? */
    mpu_initure.IsCacheable = cen;                            /* �Ƿ�cache? */
    mpu_initure.IsBufferable = ben;                           /* �Ƿ񻺳�? */
    HAL_MPU_ConfigRegion(&mpu_initure);                       /* ����MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);                   /* ����MPU */
    return 0;
}

/**
 * @brief       ������Ҫ�����Ĵ洢��
 * @param       ��
 * @note        ����Բ��ִ洢�������MPU����,������ܵ��³��������쳣
 *              ����MCU������ʾ,����ͷ�ɼ����ݳ���ȵ�����...
 */
void mpu_memory_protection(void)
{
    /* ��������D1 SRAM 512KB */
    mpu_set_protection( 0x24000000,                 /* ����ַ */
                        MPU_REGION_SIZE_512KB,      /* ���� */
                        MPU_REGION_NUMBER1,         /* NUMER1 */
                        MPU_REGION_FULL_ACCESS,     /* ȫ���� */
                        MPU_ACCESS_SHAREABLE,       /* ������ */
                        MPU_ACCESS_CACHEABLE,       /* ����cache */
                        MPU_ACCESS_NOT_BUFFERABLE); /* ��ֹ���� */
    
    /* ����SDRAM����,��32M�ֽ� */
    mpu_set_protection( 0XC0000000,                 /* ����ַ */
                        MPU_REGION_SIZE_32MB,       /* ���� */
                        MPU_REGION_NUMBER2,         /* NUMER2 */
                        MPU_REGION_FULL_ACCESS,     /* ȫ���� */
                        MPU_ACCESS_NOT_SHAREABLE,   /* ��ֹ���� */
                        MPU_ACCESS_CACHEABLE,       /* ����cache */
                        MPU_ACCESS_BUFFERABLE);     /* ������ */
}

