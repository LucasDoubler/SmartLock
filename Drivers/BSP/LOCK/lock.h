#if !defined(__LOCK__H)
#define __LOCK__H
#include "./SYSTEM/sys/sys.h"
#include "mbedtls/aes.h"
#include "stm32h7xx_hal.h"

void RNG_Init(void);
void Generate_AES_Key(uint8_t *aes_key);
void aes_init(mbedtls_aes_context *aes_ctx);
void aes_free(mbedtls_aes_context *aes_ctx);
int aes_encrypt_with_padding(uint8_t *input, size_t input_len, uint8_t *output, uint8_t *key, uint8_t *iv);
int aes_decrypt_with_padding(uint8_t *input, size_t input_len, uint8_t *output, uint8_t *key, uint8_t *iv);
int verify_password(uint8_t *stored_encrypted_password, size_t encrypted_len, uint8_t *input_password, uint8_t *key, uint8_t *iv);


#define TRIG_PIN GPIO_PIN_8 // TRIG 引脚 (PB8)
#define TRIG_PORT GPIOB     // TRIG 所在端口

#define ECHO_PIN GPIO_PIN_9 // ECHO 引脚 (PB9)
#define ECHO_PORT GPIOB     // ECHO 所在端口

#define TIMER_INSTANCE TIM4 // 使用 TIM4 作为捕获定时器
void HCSR04_Timer_Init(void);
void HCSR04_GPIO_Init(void);
float HCSR04_Measure_Distance(void);

void SG90_GPIO_Init(void);
void MX_TIM2_Init(void);
void SG90_Rotate90(void);


#endif // __LOCK__H
