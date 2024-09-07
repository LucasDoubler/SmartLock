#include "stm32h7xx_hal.h"
#include "./BSP/DHT22/dht22.h"
#include "./SYSTEM/delay/delay.h"

uint8_t DHT_buffer[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
uint32_t sum = 0;
/**
 * @brief 初始化DHT22传感器的指定模式。
 *
 * @param Mode 要初始化传感器的模式。
 */
void DHT22_Init(uint32_t Mode)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    // 开启GPIOB时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_PIN_12;              // PB10引脚配置
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;    // IO口速度为低频
    GPIO_InitStructure.Pull = GPIO_NOPULL;             // 不使用上下拉电阻

    if (Mode == GPIO_MODE_OUTPUT_OD) {
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD; // 开漏输出
    } else if (Mode == GPIO_MODE_INPUT) {
        GPIO_InitStructure.Mode = GPIO_MODE_INPUT;     // 输入模式
    }

    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure); // 初始化GPIOB
}

/**
 * @brief 从DHT22传感器读取数据
 * @retval 读取到的数据
 */
uint8_t DHT22_GetData()
{
    uint8_t retry = 0;
    uint8_t temporary = 0;
    uint8_t readData = 0;

    for (int i = 0; i < 8; i++) {
        // 等待信号从低电平变为高电平
        while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET && retry < 100) {
            delay_us(1);
            retry++;
        }
        retry = 0;
        temporary = 0;
        delay_us(30);

        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET) {
            temporary = 1;
        }
        // 等待信号从高电平变为低电平
        while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET && retry < 100) {
            delay_us(1);
            retry++;
        }
        retry = 0;
        readData <<= 1;
        readData |= temporary;
    }

    return readData;
}
/**
 * @brief 读取DHT22传感器数据
 * @retval 读取成功返回1，读取失败返回0
 */

uint8_t DHT22_Read()
{
    uint8_t j;

    DHT22_Init(GPIO_MODE_OUTPUT_OD);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    delay_ms(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    delay_us(35);
    DHT22_Init(GPIO_MODE_INPUT);
    delay_us(20);

    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET) {
        delay_us(160);
        for (j = 0; j < 5; j++) {
            DHT_buffer[j] = DHT22_GetData();
        }
        delay_us(50);
    }

    sum = (DHT_buffer[0] + DHT_buffer[1] + DHT_buffer[2] + DHT_buffer[3]);
    if (DHT_buffer[4] == (uint8_t)(sum)) {
        return 1;
    } else {
        return 0;
    }
}
