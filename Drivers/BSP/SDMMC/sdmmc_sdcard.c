

#include "string.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"


SD_HandleTypeDef g_sd_handle;                 /* SD����� */
HAL_SD_CardInfoTypeDef g_sd_card_info_handle; /* SD����Ϣ�ṹ�� */

/* sdmmc_read_disk/sdmmc_write_disk����ר��buf,�����������������ݻ�������ַ����4�ֽڶ����ʱ��,
 * ��Ҫ�õ�������,ȷ�����ݻ�������ַ��4�ֽڶ����.
 */
__ALIGNED(4) uint8_t g_sd_data_buffer[512];

/**
 * @brief       ��ʼ��SD��
 * @param       ��
 * @retval      ����ֵ:0 ��ʼ����ȷ������ֵ����ʼ������
 */
uint8_t sd_init(void)
{
    uint8_t SD_Error;

    /* ��ʼ��ʱ��ʱ�Ӳ��ܴ���400KHZ */
    g_sd_handle.Instance = SDMMC1;
    g_sd_handle.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;                       /* ������ */
    g_sd_handle.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;           /* ����ʱ���ر�ʱ�ӵ�Դ */
    g_sd_handle.Init.BusWide = SDMMC_BUS_WIDE_4B;                               /* 4λ������ */
    g_sd_handle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE; /* �ر�Ӳ������ */
    g_sd_handle.Init.ClockDiv = SDMMC_NSpeed_CLK_DIV;                           /* SD����ʱ��Ƶ�����25MHZ */

    SD_Error = HAL_SD_Init(&g_sd_handle);
    if (SD_Error != HAL_OK)
    {
        return 1;
    }
    
    HAL_SD_GetCardInfo(&g_sd_handle, &g_sd_card_info_handle);                   /* ��ȡSD����Ϣ */

    return 0;
}

/**
 * @brief       SDMMC�ײ�������ʱ��ʹ�ܣ��������ã�DMA����
                �˺����ᱻHAL_SD_Init()����
 * @param       hsd:SD�����
 * @retval      ��
 */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    if (hsd->Instance == SDMMC1)
    {
        GPIO_InitTypeDef gpio_init_struct;

        __HAL_RCC_SDMMC1_CLK_ENABLE();                       /* ʹ��SDMMC1ʱ�� */
        SD1_D0_GPIO_CLK_ENABLE();                            /* D0����IOʱ��ʹ�� */
        SD1_D1_GPIO_CLK_ENABLE();                            /* D1����IOʱ��ʹ�� */
        SD1_D2_GPIO_CLK_ENABLE();                            /* D2����IOʱ��ʹ�� */
        SD1_D3_GPIO_CLK_ENABLE();                            /* D3����IOʱ��ʹ�� */
        SD1_CLK_GPIO_CLK_ENABLE();                           /* CLK����IOʱ��ʹ�� */
        SD1_CMD_GPIO_CLK_ENABLE();                           /* CMD����IOʱ��ʹ�� */

        /* SD1_D0����ģʽ���� */
        gpio_init_struct.Pin = SD1_D0_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;             /* ���츴�� */
        gpio_init_struct.Pull = GPIO_PULLUP;                 /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;       /* ���� */
        gpio_init_struct.Alternate = GPIO_AF12_SDIO1;        /* ����ΪSDIO */
        HAL_GPIO_Init(SD1_D0_GPIO_PORT, &gpio_init_struct);  /* ��ʼ�� */

        gpio_init_struct.Pin = SD1_D1_GPIO_PIN;
        HAL_GPIO_Init(SD1_D1_GPIO_PORT, &gpio_init_struct);  /* ��ʼ��SD1_D1���� */

        gpio_init_struct.Pin = SD1_D2_GPIO_PIN;
        HAL_GPIO_Init(SD1_D2_GPIO_PORT, &gpio_init_struct);  /* ��ʼ��SD2_D1���� */

        gpio_init_struct.Pin = SD1_D3_GPIO_PIN;
        HAL_GPIO_Init(SD1_D3_GPIO_PORT, &gpio_init_struct);  /* ��ʼ��SD3_D1���� */

        gpio_init_struct.Pin = SD1_CLK_GPIO_PIN;
        HAL_GPIO_Init(SD1_CLK_GPIO_PORT, &gpio_init_struct); /* ��ʼ��SD4_D1���� */

        gpio_init_struct.Pin = SD1_CMD_GPIO_PIN;
        HAL_GPIO_Init(SD1_CMD_GPIO_PORT, &gpio_init_struct); /* ��ʼ��SD5_D1���� */
    }
}

/**
 * @brief       ��ȡ����Ϣ����
 * @param       cardinfo:SD����Ϣ���
 * @retval      ����ֵ:��ȡ����Ϣ״ֵ̬
 */
uint8_t get_sd_card_info(HAL_SD_CardInfoTypeDef *cardinfo)
{
    uint8_t sta;
    sta = HAL_SD_GetCardInfo(&g_sd_handle, cardinfo);
    return sta;
}

/**
 * @brief       �ж�SD���Ƿ���Դ���(��д)����
 * @param       ��
 * @retval      ����ֵ:SD_TRANSFER_OK      ������ɣ����Լ�����һ�δ���
                       SD_TRANSFER_BUSY SD ����æ�������Խ�����һ�δ���
 */
uint8_t get_sd_card_state(void)
{
    return ((HAL_SD_GetCardState(&g_sd_handle) == HAL_SD_CARD_TRANSFER) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/**
 * @brief       ��SD��(fatfs/usb����)
 * @param       pbuf  : ���ݻ�����
 * @param       saddr : ������ַ
 * @param       cnt   : ��������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    sys_intx_disable();                                                                    /* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!) */
    sta = HAL_SD_ReadBlocks(&g_sd_handle, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT);      /* ���sector�Ķ����� */

    /* �ȴ�SD������ */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }

    sys_intx_enable(); /* �������ж� */
    return sta;
}

/**
 * @brief       дSD��(fatfs/usb����)
 * @param       pbuf  : ���ݻ�����
 * @param       saddr : ������ַ
 * @param       cnt   : ��������
 * @retval      0, ����;  ����, �������(���SD_Error����);
 */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t sta = HAL_OK;
    uint32_t timeout = SD_TIMEOUT;
    long long lsector = saddr;
    sys_intx_disable();                                                                 /* �ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!) */
    sta = HAL_SD_WriteBlocks(&g_sd_handle, (uint8_t *)pbuf, lsector, cnt, SD_TIMEOUT);  /* ���sector��д���� */

    /* �ȴ�SD��д�� */
    while (get_sd_card_state() != SD_TRANSFER_OK)
    {
        if (timeout-- == 0)
        {
            sta = SD_TRANSFER_BUSY;
        }
    }

    sys_intx_enable();     /* �������ж� */
    
    return sta;
}
