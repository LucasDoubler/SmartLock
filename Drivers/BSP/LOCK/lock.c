#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_rng.h"
#include "ff.h"
#include "stdbool.h"
#include <string.h>
#include "mbedtls/aes.h"
#include "./BSP/LOCK/lock.h"
#include "./SYSTEM/delay/delay.h"
#include "BSP/LED/led.h"
#include "FreeRTOS.h"
#define AES_BLOCK_SIZE 16

// RNG 句柄
RNG_HandleTypeDef hrng;
// 定时器句柄
TIM_HandleTypeDef htim4;

// 初始化RNG外设
void RNG_Init(void)
{
    // 启用RNG时钟
    __HAL_RCC_RNG_CLK_ENABLE();

    // 配置RNG
    hrng.Instance = RNG;
    HAL_RNG_Init(&hrng);
}

/**
 * @brief   使用RNG生成128位（16字节）的AES密钥,也可以用来生成IV
 *
 * @param aes_key   生成的AES密钥
 */
void Generate_AES_Key(uint8_t *aes_key)
{
    for (int i = 0; i < 4; i++)
    { // 生成16字节密钥，每次生成4字节（32位）
        uint32_t random_number = 0;

        // 生成随机数
        if (HAL_RNG_GenerateRandomNumber(&hrng, &random_number) == HAL_OK)
        {
            aes_key[i * 4] = (random_number >> 24) & 0xFF;
            aes_key[i * 4 + 1] = (random_number >> 16) & 0xFF;
            aes_key[i * 4 + 2] = (random_number >> 8) & 0xFF;
            aes_key[i * 4 + 3] = random_number & 0xFF;
        }
    }
}

void aes_init(mbedtls_aes_context *aes_ctx)
{
    mbedtls_aes_init(aes_ctx);
}

void aes_free(mbedtls_aes_context *aes_ctx)
{
    mbedtls_aes_free(aes_ctx);
}

// PKCS#7 填充
void pkcs7_pad(uint8_t *input, size_t input_len, size_t block_size)
{
    size_t padding_len = block_size - (input_len % block_size);
    for (size_t i = 0; i < padding_len; i++)
    {
        input[input_len + i] = (uint8_t)padding_len;
    }
}

/**
 * @brief 使用AES算法对输入数据进行填充加密
 *
 * @param input 输入数据的指针
 * @param input_len 输入数据的长度
 * @param output 输出数据的指针
 * @param key 加密密钥的指针
 * @param iv 初始化向量的指针
 * @return int 返回加密结果，0表示成功，负数表示错误
 */
int aes_encrypt_with_padding(uint8_t *input, size_t input_len, uint8_t *output, uint8_t *key, uint8_t *iv)
{
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);                                              // 初始化AES上下文
    size_t padded_len = AES_BLOCK_SIZE * ((input_len / AES_BLOCK_SIZE) + 1); // 计算填充后的长度

    uint8_t *temp_input = (uint8_t *)pvPortMalloc(padded_len);
    if (temp_input == NULL)
    {
        // 处理内存分配失败的情况
        return -1;
    }
    memset(temp_input, 0, padded_len);    // 初始化填充后的输入数据
    memcpy(temp_input, input, input_len); // 复制输入数据

    // 添加PKCS#7填充
    pkcs7_pad(temp_input, input_len, AES_BLOCK_SIZE);

    // 设置加密密钥
    int ret = mbedtls_aes_setkey_enc(&aes_ctx, key, 128);
    if (ret != 0)
    {
        mbedtls_aes_free(&aes_ctx);
        vPortFree(temp_input); // 操作完成后释放内存
        return ret;
    }

    // 重新初始化IV，以确保每次加密使用相同的IV
    uint8_t iv_copy[16];
    memcpy(iv_copy, iv, 16);

    // 加密操作
    ret = mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, padded_len, iv_copy, temp_input, output);

    // 释放资源
    mbedtls_aes_free(&aes_ctx);
    vPortFree(temp_input);
    return ret;
}

// 去除PKCS#7 填充
int pkcs7_unpad(uint8_t *input, size_t input_len)
{
    if (input_len == 0 || input_len % AES_BLOCK_SIZE != 0)
    {
        return -1; // 无效的输入长度
    }

    uint8_t padding_len = input[input_len - 1];
    if (padding_len > AES_BLOCK_SIZE)
    {
        return -1; // 无效的填充
    }

    for (size_t i = 0; i < padding_len; i++)
    {
        if (input[input_len - 1 - i] != padding_len)
        {
            return -1; // 无效的填充内容
        }
    }

    return input_len - padding_len;
}

/**
 * @brief 使用AES解密算法对输入数据进行解密，并去除填充。
 *
 * @param input 输入数据的指针。
 * @param input_len 输入数据的长度。
 * @param output 输出数据的指针。
 * @param key 解密密钥。
 * @param iv 初始化向量。
 * @return 解密后的数据长度，如果解密失败则返回负值。
 */
int aes_decrypt_with_padding(uint8_t *input, size_t input_len, uint8_t *output, uint8_t *key, uint8_t *iv)
{
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx); // 初始化AES上下文

    int ret = mbedtls_aes_setkey_dec(&aes_ctx, key, 128); // 设置解密密钥
    if (ret != 0)
    {
        mbedtls_aes_free(&aes_ctx);
        return ret;
    }

    uint8_t iv_copy[16];
    memcpy(iv_copy, iv, sizeof(iv_copy)); // 确保IV的复制

    ret = mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, input_len, iv_copy, input, output);
    if (ret != 0)
    {
        mbedtls_aes_free(&aes_ctx);
        return ret;
    }

    // 去除填充
    int unpadded_len = pkcs7_unpad(output, input_len);
    if (unpadded_len < 0)
    {
        mbedtls_aes_free(&aes_ctx);
        return -1; // 填充错误
    }

    mbedtls_aes_free(&aes_ctx);
    return unpadded_len;
}

// 验证函数
int verify_password(uint8_t *stored_encrypted_password, size_t encrypted_len, uint8_t *input_password, uint8_t *key, uint8_t *iv)
{
    // 解密存储的密码
    uint8_t decrypted_password[encrypted_len];
    int decrypted_len = aes_decrypt_with_padding(stored_encrypted_password, encrypted_len, decrypted_password, key, iv);
    if (decrypted_len < 0)
    {
        printf("解密失败\n");
        return 0; // 返回0表示验证失败
    }

    // 比较解密后的密码与用户输入的密码
    if (decrypted_len == strlen((char *)input_password) && memcmp(decrypted_password, input_password, decrypted_len) == 0)
    {
        return 1; // 返回1表示密码正确
    }
    else
    {
        return 0; // 返回0表示密码错误
    }
}

/**
 * @brief 采用HC-SR04超声波传感器测量距离,检测门的开关状态
 *
 */

TIM_HandleTypeDef htim4;

// 初始化 GPIO 引脚
void HCSR04_GPIO_Init(void)
{
    // 启用 GPIOB 时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 配置 TRIG 引脚为输出
    GPIO_InitStruct.Pin = TRIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TRIG_PORT, &GPIO_InitStruct);

    // 配置 ECHO 引脚为输入捕获通道 (TIM4_CH4)
    GPIO_InitStruct.Pin = ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4; // 设置为 TIM4 的复用功能
    HAL_GPIO_Init(ECHO_PORT, &GPIO_InitStruct);
}

// 初始化 Timer4 用于捕获 ECHO 引脚的高电平时长
void HCSR04_Timer_Init(void)
{
    TIM_IC_InitTypeDef sConfigIC = {0};

    __HAL_RCC_TIM4_CLK_ENABLE(); // 启用 TIM4 定时器时钟

    htim4.Instance = TIMER_INSTANCE;
    htim4.Init.Prescaler = (480000000 / 1000000) - 1; // 1 MHz 频率（1us 计时）
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 0xFFFF; // 最大计数值
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_IC_Init(&htim4);

    // 配置 TIM4 的输入捕获通道4 (PB9 - ECHO)
    sConfigIC.ICPolarity = TIM_ICPOLARITY_RISING; // 首先捕获上升沿
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_4);

    // 启动输入捕获
    HAL_TIM_IC_Start(&htim4, TIM_CHANNEL_4);
}
float HCSR04_Measure_Distance(void)
{
    uint32_t start_time, stop_time;
    float distance;

    // 发出 TRIG 信号：保持低电平 2us，随后拉高 10us，再拉低
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
    delay_us(2); // 延时 2us
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
    delay_us(10); // 保持高电平 10us
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

    // 等待 ECHO 引脚拉高（上升沿）
    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_RESET)
        ;

    // 记录开始时间
    start_time = __HAL_TIM_GET_COUNTER(&htim4);

    // 等待 ECHO 引脚拉低（下降沿）
    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_SET)
        ;

    // 记录结束时间
    stop_time = __HAL_TIM_GET_COUNTER(&htim4);

    // 计算 ECHO 高电平的持续时间 (us)
    uint32_t echo_duration = stop_time - start_time;
    /**
     * @attention 计算公式本应该是时间echo_duration乘声速除以2，但是由于声速是343m/s，但是由于位置原因，时间echo_duration是实际时间的一半
     *
     */
    // 计算距离（声速约为 343 m/s）
    distance = echo_duration * 0.0343f; // 单位：厘米

    return distance;
}

TIM_HandleTypeDef htim2;

/* TIM2 初始化函数 */
void MX_TIM2_Init(void)
{
    // 使能 TIM2 时钟
    __HAL_RCC_TIM2_CLK_ENABLE();

    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    // 初始化定时器参数
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 239; // 将时钟频率 240MHz 降到 1MHz (240MHz / 240 = 1MHz)
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 19999; // PWM周期为20ms (1MHz / 20000 = 50Hz)
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim2); // 初始化 PWM

    // 配置 PWM 输出通道2
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1500; // 初始占空比设置为 1500 (对应中位角度)
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);

    // 主模式配置
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
}

/* PA1 GPIO 配置 */
void SG90_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能 GPIOA 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 将 PA1 配置为复用模式 (TIM2_CH2)
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2; // PA1 复用为 TIM2_CH2
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
/**
 * @brief SG90 舵机旋转90度，因为使用的是360舵机，所以无法准确设定旋转角度
 *
 */
void SG90_Rotate90(void)
{ // 启动 PWM 输出
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1000);
    delay_ms(175);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
}
