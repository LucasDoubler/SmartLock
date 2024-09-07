

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "BSP/ESP8266/esp8266.h"
#include "FreeRTOS.h"
#include "semphr.h"
/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
// #include "os.h"   /* os 使用 */
#endif

/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1
#if (__ARMCC_VERSION >= 6010050)           /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t"); /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");   /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE 在 stdio.h里面定义 */
FILE __stdout;

/* 重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART_UX->ISR & 0X40) == 0)
        ; /* 等待上一个字符发送完成 */

    USART_UX->TDR = (uint8_t)ch; /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif

/******************************************************************************************/

#if USART_EN_RX /* 如果使能了接收 */

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
 */
uint16_t g_usart_rx_sta = 0;
uint8_t g_rx_buffer[RXBUFFERSIZE];     /* HAL库使用的串口接收缓冲 */
uint8_t g_usart_rx_buf[USART_REC_LEN]; /* 接收缓冲, 最大USART_REC_LEN个字节. */

uint16_t g_usart3_rx_sta = 0;
uint8_t g_usart3_rx_buffer[RXBUFFERSIZE]; /* HAL库使用的串口接收缓冲 */
uint8_t g_usart3_rx_buf[USART_REC_LEN];   /* 接收缓冲, 最大USART_REC_LEN个字节. */
UART_HandleTypeDef g_uart1_handle;        /* UART句柄 */
UART_HandleTypeDef huart3;

extern char json_data[MAX_JSON_LENGTH];

extern SemaphoreHandle_t xChangeSemaphore;
volatile uint8_t at_cmd_result = 0;      // 0: 未处理, 1: 成功, 2: 失败, 3: 超时
volatile uint32_t at_cmd_start_time = 0; // AT指令发送的时间戳
/**
 * @brief       串口X初始化函数
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @note        注意: 必须设置正确的时钟源, 否则串口波特率就会设置异常.
 *              这里的USART的时钟源在sys_stm32_clock_init()函数中已经设置过了.
 * @retval      无
 */
void usart1_init(uint32_t baudrate)
{
    g_uart1_handle.Instance = USART_UX;                  /* USART1 */
    g_uart1_handle.Init.BaudRate = baudrate;             /* 波特率 */
    g_uart1_handle.Init.WordLength = UART_WORDLENGTH_8B; /* 字长为8位数据格式 */
    g_uart1_handle.Init.StopBits = UART_STOPBITS_1;      /* 一个停止位 */
    g_uart1_handle.Init.Parity = UART_PARITY_NONE;       /* 无奇偶校验位 */
    g_uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* 无硬件流控 */
    g_uart1_handle.Init.Mode = UART_MODE_TX_RX;          /* 收发模式 */
    HAL_UART_Init(&g_uart1_handle);                      /* HAL_UART_Init()会使能UART1 */

    /* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
    HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
}
void usart3_init(uint32_t baudrate)
{
    huart3.Instance = USART3;                        // 使用USART3
    huart3.Init.BaudRate = baudrate;                 // 设置波特率为9600
    huart3.Init.WordLength = UART_WORDLENGTH_8B;     // 字长8位
    huart3.Init.StopBits = UART_STOPBITS_1;          // 1个停止位
    huart3.Init.Parity = UART_PARITY_NONE;           // 无校验位
    huart3.Init.Mode = UART_MODE_TX_RX;              // 配置为发送和接收模式
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;     // 无硬件流控
    huart3.Init.OverSampling = UART_OVERSAMPLING_16; // 过采样16倍
    HAL_UART_Init(&huart3);
    HAL_UART_Receive_IT(&huart3, (uint8_t *)g_usart3_rx_buffer, RXBUFFERSIZE);
}

void UART3_Transmit(char *command)
{
    // Assuming HAL_UART_Transmit is used for blocking mode transmission
    HAL_UART_Transmit(&huart3, (uint8_t *)command, strlen(command), HAL_MAX_DELAY);
}
/**
 * @brief       串口发送数据
 * @param       data: 待发送数据的指针
 * @param       size: 待发送数据的大小
 * @retval      无
 */
uint8_t UART3_Send_AT_Command(char *command, uint32_t timeout_ms)
{
    at_cmd_result = 0;                 // 重置AT命令状态
    at_cmd_start_time = HAL_GetTick(); // 获取当前时间

    UART3_Transmit(command);                                                   // 发送AT指令
    HAL_UART_Receive_IT(&huart3, (uint8_t *)g_usart3_rx_buffer, RXBUFFERSIZE); // 开启中断接收

    // 等待响应或超时
    while (at_cmd_result == 0)
    {
        if ((HAL_GetTick() - at_cmd_start_time) > timeout_ms) // 超时检测
        {
            at_cmd_result = 3; // 超时
            break;
        }
    }

    return at_cmd_result; // 返回结果，1: 成功, 2: 失败, 3: 超时
}
/**
 * @brief       UART底层初始化函数
 * @param       huart: UART句柄类型指针
 * @note        此函数会被HAL_UART_Init()调用
 *              完成时钟使能，引脚配置，中断配置
 * @retval      无
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
    if (huart->Instance == USART_UX) /* 如果是串口1，进行串口1 MSP初始化 */
    {
        USART_UX_CLK_ENABLE();      /* USART1 时钟使能 */
        USART_TX_GPIO_CLK_ENABLE(); /* 发送引脚时钟使能 */
        USART_RX_GPIO_CLK_ENABLE(); /* 接收引脚时钟使能 */

        gpio_init_struct.Pin = USART_TX_GPIO_PIN;             /* TX引脚 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;              /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                  /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;        /* 高速 */
        gpio_init_struct.Alternate = USART_TX_GPIO_AF;        /* 复用为USART1 */
        HAL_GPIO_Init(USART_TX_GPIO_PORT, &gpio_init_struct); /* 初始化发送引脚 */

        gpio_init_struct.Pin = USART_RX_GPIO_PIN;             /* RX引脚 */
        gpio_init_struct.Alternate = USART_RX_GPIO_AF;        /* 复用为USART1 */
        HAL_GPIO_Init(USART_RX_GPIO_PORT, &gpio_init_struct); /* 初始化接收引脚 */

#if USART_EN_RX
        HAL_NVIC_EnableIRQ(USART_UX_IRQn);         /* 使能USART1中断通道 */
        HAL_NVIC_SetPriority(USART_UX_IRQn, 3, 3); /* 抢占优先级3，子优先级3 */

#endif
    }
    else if (huart->Instance == USART3)
    {

        // 使能USART3时钟
        __HAL_RCC_USART3_CLK_ENABLE();

        // 使能GPIOB时钟
        __HAL_RCC_GPIOB_CLK_ENABLE();

        // 配置PB10为USART3 TX
        gpio_init_struct.Pin = GPIO_PIN_10;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_NOPULL;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio_init_struct.Alternate = GPIO_AF7_USART3; // 复用为USART3_TX
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);

        // 配置PB11为USART3 RX
        gpio_init_struct.Pin = GPIO_PIN_11;
        gpio_init_struct.Alternate = GPIO_AF7_USART3; // 复用为USART3_RX
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);

        // 配置NVIC中断
        HAL_NVIC_EnableIRQ(USART3_IRQn);
        HAL_NVIC_SetPriority(USART3_IRQn, 2, 3);
    }
}

/**
 * @brief       Rx传输回调函数
 * @param       huart: UART句柄类型指针
 * @retval      无
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) /* 如果是串口1 */
    {
        if ((g_usart_rx_sta & 0x8000) == 0) /* 接收未完成 */
        {
            if (g_usart_rx_sta & 0x4000) /* 接收到了0x0d */
            {
                if (g_rx_buffer[0] != 0x0a)
                {
                    g_usart_rx_sta = 0; /* 接收错误,重新开始 */
                }
                else
                {
                    g_usart_rx_sta |= 0x8000; /* 接收完成了 */
                }
            }
            else /* 还没收到0X0D */
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
                        g_usart_rx_sta = 0; /* 接收数据错误,重新开始接收 */
                    }
                }
            }
        }
        HAL_UART_Receive_IT(&g_uart1_handle, (uint8_t *)g_rx_buffer, RXBUFFERSIZE);
    }
    else if (huart->Instance == USART3)
    {
        // 直接将接收到的数据通过 printf 打印输出
        if ((g_usart3_rx_sta & 0x8000) == 0) /* 接收未完成 */
        {
            if (g_usart3_rx_sta & 0x4000) /* 已接收到0x0D */
            {
                if (g_usart3_rx_buffer[0] != 0x0A)
                {
                    g_usart3_rx_sta = 0; /* 接收错误,重新开始 */
                }
                else if (g_usart3_rx_buffer[0] == 0x0A)
                {
                    g_usart3_rx_sta |= 0x8000; /* 接收完成 */
                                               // 解析返回数据，检查是否包含 "OK" 或 "ERROR"
                                               // 打印接收到的蓝牙数据
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
                    // 清空缓冲区并重置接收状态标志位
                    memset(g_usart3_rx_buf, 0, sizeof(g_usart3_rx_buf));
                    g_usart3_rx_sta = 0;
                    if (strstr((char *)g_usart3_rx_buf, "OK") != NULL)
                    {
                        at_cmd_result = 1; // 指令执行成功
                    }
                    else if (strstr((char *)g_usart3_rx_buf, "ERROR") != NULL)
                    {
                        at_cmd_result = 2; // 指令执行失败
                    }
                }
            }
            else /* 尚未接收到0X0D */
            {
                if (g_usart3_rx_buffer[0] == 0x0D)
                {
                    g_usart3_rx_sta |= 0x4000; /* 标记已接收到0x0D */
                }
                else
                {
                    uint16_t rx_len = g_usart3_rx_sta & 0x3FFF;
                    g_usart3_rx_buf[rx_len] = g_usart3_rx_buffer[0];
                    g_usart3_rx_sta++;
                    if (rx_len >= (USART_REC_LEN - 1))
                    {
                        g_usart3_rx_sta = 0; /* 接收数据错误,重新开始 */
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
 * @brief       串口1中断服务函数
 * @param       无
 * @retval      无
 */
void USART_UX_IRQHandler(void)
{
#if SYS_SUPPORT_OS /* 使用OS */
//    OSIntEnter();
#endif

    HAL_UART_IRQHandler(&g_uart1_handle); /* 调用HAL库中断处理公用函数 */

#if SYS_SUPPORT_OS /* 使用OS */
//    OSIntExit();
#endif
}

#endif
