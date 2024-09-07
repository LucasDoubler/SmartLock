

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "BSP/ESP8266/esp8266.h"
#include "FreeRTOS.h"
#include "semphr.h"
/* ���ʹ��os,����������ͷ�ļ�����. */
#if SYS_SUPPORT_OS
// #include "os.h"   /* os ʹ�� */
#endif

/******************************************************************************************/
/* �������´���, ֧��printf����, ������Ҫѡ��use MicroLIB */

#if 1
#if (__ARMCC_VERSION >= 6010050)           /* ʹ��AC6������ʱ */
__asm(".global __use_no_semihosting\n\t"); /* ������ʹ�ð�����ģʽ */
__asm(".global __ARM_use_no_argv \n\t");   /* AC6����Ҫ����main����Ϊ�޲�����ʽ�����򲿷����̿��ܳ��ְ�����ģʽ */

#else
/* ʹ��AC5������ʱ, Ҫ�����ﶨ��__FILE �� ��ʹ�ð�����ģʽ */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* ��ʹ�ð�����ģʽ��������Ҫ�ض���_ttywrch\_sys_exit\_sys_command_string����,��ͬʱ����AC6��AC5ģʽ */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* ����_sys_exit()�Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE �� stdio.h���涨�� */
FILE __stdout;

/* �ض���fputc����, printf�������ջ�ͨ������fputc����ַ��������� */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->ISR & 0X40) == 0)
        ; /* �ȴ���һ���ַ�������� */

    USART_UX->TDR = (uint8_t)ch; /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    return ch;
}
#endif

/******************************************************************************************/

#if USART_EN_RX /* ���ʹ���˽��� */

/*  ����״̬
 *  bit15��      ������ɱ�־
 *  bit14��      ���յ�0x0d
 *  bit13~0��    ���յ�����Ч�ֽ���Ŀ
 */
uint16_t g_usart_rx_sta = 0;
uint8_t g_rx_buffer[RXBUFFERSIZE];     /* HAL��ʹ�õĴ��ڽ��ջ��� */
uint8_t g_usart_rx_buf[USART_REC_LEN]; /* ���ջ���, ���USART_REC_LEN���ֽ�. */

uint16_t g_usart3_rx_sta = 0;
uint8_t g_usart3_rx_buffer[RXBUFFERSIZE]; /* HAL��ʹ�õĴ��ڽ��ջ��� */
uint8_t g_usart3_rx_buf[USART_REC_LEN];   /* ���ջ���, ���USART_REC_LEN���ֽ�. */
UART_HandleTypeDef g_uart1_handle;        /* UART��� */
UART_HandleTypeDef huart3;

extern char json_data[MAX_JSON_LENGTH];

extern SemaphoreHandle_t xChangeSemaphore;
volatile uint8_t at_cmd_result = 0;      // 0: δ����, 1: �ɹ�, 2: ʧ��, 3: ��ʱ
volatile uint32_t at_cmd_start_time = 0; // ATָ��͵�ʱ���
/**
 * @brief       ����X��ʼ������
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @note        ע��: ����������ȷ��ʱ��Դ, ���򴮿ڲ����ʾͻ������쳣.
 *              �����USART��ʱ��Դ��sys_stm32_clock_init()�������Ѿ����ù���.
 * @retval      ��
 */
void usart1_init(uint32_t baudrate)
{
    g_uart1_handle.Instance = USART_UX;                  /* USART1 */
    g_uart1_handle.Init.BaudRate = baudrate;             /* ������ */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B; /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1;      /* һ��ֹͣλ */
    g_uart1_handle.Init.Parity = UART_PARITY_NONE;       /* ����żУ��λ */
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* ��Ӳ������ */
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX;          /* �շ�ģʽ */
    HAL_UART_Init(&g_uart1_handle);                      /* HAL_UART_Init()��ʹ��UART1 */

    /* �ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ��������������� */
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
}
void usart3_init(uint32_t baudrate)
{
    huart3.Instance = USART3;                        // ʹ��USART3
    huart3.Init.BaudRate = baudrate;                 // ���ò�����Ϊ9600
    huart3.Init.WordLength = UART_WORDLENGTH_8B;     // �ֳ�8λ
    huart3.Init.StopBits = UART_STOPBITS_1;          // 1��ֹͣλ
    huart3.Init.Parity = UART_PARITY_NONE;           // ��У��λ
    huart3.Init.Mode = UART_MODE_TX_RX;              // ����Ϊ���ͺͽ���ģʽ
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;     // ��Ӳ������
    huart3.Init.OverSampling = UART_OVERSAMPLING_16; // ������16��
    HAL_UART_Init(&huart3);
    HAL_UART_Receive_IT(&huart3, (uint8_t *)g_usart3_rx_buffer, RXBUFFERSIZE);
}

void UART3_Transmit(char *command)
{
    // Assuming HAL_UART_Transmit is used for blocking mode transmission
    HAL_UART_Transmit(&huart3, (uint8_t *)command, strlen(command), HAL_MAX_DELAY);
}
/**
 * @brief       ���ڷ�������
 * @param       data: ���������ݵ�ָ��
 * @param       size: ���������ݵĴ�С
 * @retval      ��
 */
uint8_t UART3_Send_AT_Command(char *command, uint32_t timeout_ms)
{
    at_cmd_result = 0;                 // ����AT����״̬
    at_cmd_start_time = HAL_GetTick(); // ��ȡ��ǰʱ��

    UART3_Transmit(command);                                                   // ����ATָ��
    HAL_UART_Receive_IT(&huart3, (uint8_t *)g_usart3_rx_buffer, RXBUFFERSIZE); // �����жϽ���

    // �ȴ���Ӧ��ʱ
    while (at_cmd_result == 0)
    {
        if ((HAL_GetTick() - at_cmd_start_time) > timeout_ms) // ��ʱ���
        {
            at_cmd_result = 3; // ��ʱ
            break;
        }
    }

    return at_cmd_result; // ���ؽ����1: �ɹ�, 2: ʧ��, 3: ��ʱ
}
/**
 * @brief       UART�ײ��ʼ������
 * @param       huart: UART�������ָ��
 * @note        �˺����ᱻHAL_UART_Init()����
 *              ���ʱ��ʹ�ܣ��������ã��ж�����
 * @retval      ��
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    if (huart->Instance == USART_UX) /* ����Ǵ���1�����д���1 MSP��ʼ�� */
    {
        USART_UX_CLK_ENABLE();      /* USART1 ʱ��ʹ�� */
        USART_TX_GPIO_CLK_ENABLE(); /* ��������ʱ��ʹ�� */
        USART_RX_GPIO_CLK_ENABLE(); /* ��������ʱ��ʹ�� */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;             /* TX���� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;              /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                  /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;        /* ���� */
        gpio_init_struct.Alternate = USART_TX_GPIO_AF;        /* ����ΪUSART1 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct); /* ��ʼ���������� */

        gpio_init_struct.Pin = USART_RX_GPIO_PIN;             /* RX���� */
        gpio_init_struct.Alternate = USART_RX_GPIO_AF;        /* ����ΪUSART1 */
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct); /* ��ʼ���������� */

#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);         /* ʹ��USART1�ж�ͨ�� */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3); /* ��ռ���ȼ�3�������ȼ�3 */

#endif
    }
    else if (huart->Instance == USART3)
    {

        // ʹ��USART3ʱ��
        __HAL_RCC_USART3_CLK_ENABLE();

        // ʹ��GPIOBʱ��
        __HAL_RCC_GPIOB_CLK_ENABLE();

        // ����PB10ΪUSART3 TX
        gpio_init_struct.Pin = GPIO_PIN_10;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio_init_struct.Alternate = GPIO_AF7_USART3; // ����ΪUSART3_TX
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);

        // ����PB11ΪUSART3 RX
        gpio_init_struct.Pin = GPIO_PIN_11;
        gpio_init_struct.Alternate = GPIO_AF7_USART3; // ����ΪUSART3_RX
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);

        // ����NVIC�ж�
        HAL_NVIC_EnableIRQ(USART3_IRQn);
        HAL_NVIC_SetPriority(USART3_IRQn, 2, 3);
    }
}

/**
 * @brief       Rx����ص�����
 * @param       huart: UART�������ָ��
 * @retval      ��
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) /* ����Ǵ���1 */
    {
        if ((g_usart_rx_sta & 0x8000) == 0) /* ����δ��� */
        {
            if (g_usart_rx_sta & 0x4000) /* ���յ���0x0d */
            {
                if (g_rx_buffer[0] != 0x0a)
                {
                    g_usart_rx_sta = 0; /* ���մ���,���¿�ʼ */
                }
                else
                {
                    g_usart_rx_sta |= 0x8000; /* ��������� */
                }
            }
            else /* ��û�յ�0X0D */
            {
                if (g_rx_buffer[0] == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = g_rx_buffer[0];
                    g_usart_rx_sta++;
                    if (g_usart_rx_sta > (USART_REC_LEN - 1))
                    {
                        g_usart_rx_sta = 0; /* �������ݴ���,���¿�ʼ���� */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
    }
    else if (huart->Instance == USART3)
    {
        // ֱ�ӽ����յ�������ͨ�� printf ��ӡ���
        if ((g_usart3_rx_sta & 0x8000) == 0) /* ����δ��� */
        {
            if (g_usart3_rx_sta & 0x4000) /* �ѽ��յ�0x0D */
            {
                if (g_usart3_rx_buffer[0] != 0x0A)
                {
                    g_usart3_rx_sta = 0; /* ���մ���,���¿�ʼ */
                }
                else if (g_usart3_rx_buffer[0] == 0x0A)
                {
                    g_usart3_rx_sta |= 0x8000; /* ������� */
                                               // �����������ݣ�����Ƿ���� "OK" �� "ERROR"
                                               // ��ӡ���յ�����������
                    // printf("Received Data: %s\n", g_usart3_rx_buf);
                    uint8_t res = is_aliyun_message(g_usart3_rx_buf);
                    if (res == 1)
                    {
                        printf("is AliYun\r\n");
                        extract_json_from_mqtt_message(g_usart3_rx_buf);
                        if (parse_mqtt_message(json_data))
                        {
                            xSemaphoreGiveFromISR(xChangeSemaphore, pdFALSE);
                        }
                    }
                    // ��ջ����������ý���״̬��־λ
                    memset(g_usart3_rx_buf, 0, sizeof(g_usart3_rx_buf));
                    g_usart3_rx_sta = 0;
                    if (strstr((char *)g_usart3_rx_buf, "OK") != NULL)
                    {
                        at_cmd_result = 1; // ָ��ִ�гɹ�
                    }
                    else if (strstr((char *)g_usart3_rx_buf, "ERROR") != NULL)
                    {
                        at_cmd_result = 2; // ָ��ִ��ʧ��
                    }
                }
            }
            else /* ��δ���յ�0X0D */
            {
                if (g_usart3_rx_buffer[0] == 0x0D)
                {
                    g_usart3_rx_sta |= 0x4000; /* ����ѽ��յ�0x0D */
                }
                else
                {
                    uint16_t rx_len = g_usart3_rx_sta & 0x3FFF;
                    g_usart3_rx_buf[rx_len] = g_usart3_rx_buffer[0];
                    g_usart3_rx_sta++;
                    if (rx_len >= (USART_REC_LEN - 1))
                    {
                        g_usart3_rx_sta = 0; /* �������ݴ���,���¿�ʼ */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&huart3, (uint8_t *)g_usart3_rx_buffer, RXBUFFERSIZE);
    }
}
void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart3);
}

/**
 * @brief       ����1�жϷ�����
 * @param       ��
 * @retval      ��
 */
void USART_UX_IRQHandler(void)
{
#if SYS_SUPPORT_OS /* ʹ��OS */
//    OSIntEnter();
#endif

    HAL_UART_IRQHandler(&g_uart1_handle); /* ����HAL���жϴ����ú��� */

#if SYS_SUPPORT_OS /* ʹ��OS */
//    OSIntExit();
#endif
}

#endif
