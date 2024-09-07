#include "lvgl_demo.h"
#include "./BSP/LED/led.h"
#include "./BSP/DHT22/dht22.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_mainstart.h"
#include "semphr.h"
#include "ff.h"
#include "timers.h"
#include "./BSP/RTC/rtc.h"
#include "SYSTEM/delay/delay.h"
#include "BSP/LOCK/lock.h"
#include "BSP/ESP8266/esp8266.h"
/******************************************************************************************************/
/*自定义全局变量*/
// 全局变量

extern UART_HandleTypeDef huart3;
extern uint16_t g_usart3_rx_sta;
extern uint8_t g_usart3_rx_buf[]; /* 用于保存完整消息的较大缓冲区 */
extern lv_obj_t *label_input;
extern lv_obj_t *label_user;
extern char password[MAX_PASSWORD_LENGTH + 1]; // 存储密码
TimerHandle_t xTimer;                          // 定时器句柄

SemaphoreHandle_t xVerifySemaphore = NULL; // 验证密码的信号量

SemaphoreHandle_t xLockStatusSemaphore = NULL; // 锁状态信号量

SemaphoreHandle_t xWordUsingSemaphore = NULL; // 互斥信号量

SemaphoreHandle_t xEncryptSemaphore = NULL; // 加密信号量

SemaphoreHandle_t xConfirmSemaphore = NULL;

QueueHandle_t passwordQueue; // 定义密码队列句柄

SemaphoreHandle_t xChangeSemaphore;
lv_obj_t *pagestate;
/******************************************************************************************************/
/*自定义函数*/

volatile uint8_t changeStatus=0; // 0是没有在修改，1是正在修改
#define ENCRYPTED_FILE_PATH "encrypted_password.dat"

/**
 * @brief   从SD卡加载加密后的密码
 *
 * @param encrypted_data    加密后的数据
 * @param data_len      数据长度
 * @return int        返回0表示成功，返回-1表示失败
 */
int load_encrypted_password_from_sd(uint8_t *encrypted_data, size_t data_len)
{
    FIL file;
    FRESULT res;
    UINT bytes_read;

    // 打开文件进行读操作
    res = f_open(&file, ENCRYPTED_FILE_PATH, FA_READ);
    if (res != FR_OK)
    {
        // 文件打开失败
        printf("Failed to open file for reading: %d\n", res);
        return -1;
    }

    // 读取文件的大小
    data_len = f_size(&file);
    if (data_len == 0)
    {
        // 文件为空
        f_close(&file);
        return -1;
    }
    // 读取加密后的数据
    res = f_read(&file, encrypted_data, data_len, &bytes_read);
    if (res != FR_OK || bytes_read != data_len)
    {
        // 读取失败
        printf("Failed to read data from file: %d\n", res);
        f_close(&file);
        return -1;
    }
    // 关闭文件
    f_close(&file);
    return 0;
}

/**
 * @brief   保存加密后的密码到SD卡
 *
 * @param encrypted_data    加密后的数据
 * @param data_len        数据长度
 * @return int            返回0表示成功，返回-1表示失败
 */
int save_encrypted_password_to_sd(uint8_t *encrypted_data, size_t data_len)
{
    FIL file;
    FRESULT res;
    UINT bytes_written;

    // 打开文件进行写操作
    res = f_open(&file, ENCRYPTED_FILE_PATH, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK)
    {
        // 文件打开失败
        printf("Failed to open file for writing: %d\n", res);
        return -1;
    }

    // 写入加密后的数据到文件
    res = f_write(&file, encrypted_data, data_len, &bytes_written);
    if (res != FR_OK || bytes_written != data_len)
    {
        // 写入失败
        printf("Failed to write data to file: %d\n", res);
        f_close(&file);
        return -1;
    }

    // 关闭文件
    f_close(&file);
    return 0;
}

// 定时器回调函数
void vTimerCallback(TimerHandle_t xTimer)
{
    // 释放信号量以通知任务进行处理
    xSemaphoreGiveFromISR(xEncryptSemaphore, NULL);
}
/******************************************************************************************************/
/*FreeRTOS任务*/

/* START_TASK 任务
 * 优先级: 任务优先级 栈大小 任务句柄
 */
#define START_TASK_PRIO 1            /* 任务优先级 */
#define START_STK_SIZE 128           /* 栈大小 */
TaskHandle_t StartTask_Handler;      /* 任务句柄 */
void start_task(void *pvParameters); /* 任务函数 */

/* LV_DEMO_TASK 任务
 * 优先级: 任务优先级 栈大小 任务句柄
 */
#define LV_DEMO_TASK_PRIO 4            /* 任务优先级 */
#define LV_DEMO_STK_SIZE 512           /* 栈大小 */
TaskHandle_t LV_DEMOTask_Handler;      /* 任务句柄 */
void lv_demo_task(void *pvParameters); /* 任务函数 */

/* UI_TASK 更新LVGL传感器数据任务
 * 优先级: 任务优先级 栈大小 任务句柄
 */
#define UI_TASK_PRIO 2           /* 任务优先级 */
#define UI_STK_SIZE 128          /* 栈大小 */
TaskHandle_t UITask_Handler;     /* 任务句柄 */
void UITask(void *pvParameters); /* 任务函数 */

/* ESP8266_TASK 任务
 * 优先级: 任务优先级 栈大小 任务句柄
 */
#define ESP8266_TASK_PRIO 3            /* 任务优先级 */
#define ESP8266_STK_SIZE 128           /* 栈大小 */
TaskHandle_t ESP8266Task_Handler;      /* 任务句柄 */
void ESP8266_task(void *pvParameters); /* 任务函数 */

/* Verify_Task 任务
 * 优先级: 任务优先级 栈大小 任务句柄
 */
#define VERIFY_TASK_PRIO 8            /* 任务优先级 */
#define VERIFY_STK_SIZE 256           /* 栈大小 */
TaskHandle_t VerifyTask_Handler;      /* 任务句柄 */
void Verify_Task(void *pvParameters); /* 任务函数 */

/*LockStatus_Task 任务
 *
 */
#define LOCKSTATUS_TASK_PRIO 6            /* 任务优先级 */
#define LOCKSTATUS_STK_SIZE 128           /* 栈大小 */
TaskHandle_t LockStatusTask_Handler;      /* 任务句柄 */
void LockStatus_Task(void *pvParameters); /* 任务函数 */

/*加密任务*/
#define ENCRYPT_TASK_PRIO 5            /* 任务优先级 */
#define ENCRYPT_STK_SIZE 128           /* 栈大小 */
TaskHandle_t EncryptTask_Handler;      /* 任务句柄 */
void Encrypt_Task(void *pvParameters); /* 任务函数 */

#define CHANGEPWSD_TASK_PRIO 9
#define CHANGEPWSD_STK_SIZE 256
TaskHandle_t ChangePWSDTask_Handler;
void ChangePWSD_Task(void *pvParameters);

/******************************************************************************************************/
// 示例密钥和IV（实际使用时，应该使用更安全的方式生成密钥和IV）
uint8_t key[16];
uint8_t iv[16];
uint8_t encrypted_output[16 * 2]; // 足够大的缓冲区来存储加密后的数据
void lvgl_demo(void)
{
    lv_init();            /* lvgl系统初始化 */
    lv_port_disp_init();  /* lvgl显示接口初始化,需要在lv_init()之后调用 */
    lv_port_indev_init(); /* lvgl输入接口初始化,需要在lv_init()之后调用 */
    size_t encrypted_len;
    uint8_t password[6] = {'1', '2', '3', '4', '5', '6'};
    // uint8_t start_key[16] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F, 0x70, 0x81,
    //                          0x92, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0x07}; // 初始密钥
    // uint8_t start_iv[16] = {0x6A, 0x7B, 0x8C, 0x9D, 0x0E, 0x1F, 0x20, 0x31,
    //                         0x42, 0xA3, 0xB4, 0xC5, 0xD6, 0xE7, 0xF8, 0x0}; // 初始IV

    // load_encrypted_password_from_sd(encrypted_output, encrypted_len);                         // 读取SD卡中的密码
    // aes_decrypt_with_padding(encrypted_output, encrypted_len, password, start_key, start_iv); // 解密SD卡中的密码

    Generate_AES_Key(key); // 生成AES密钥
    Generate_AES_Key(iv);  // 生成AES IV
    encrypted_len = sizeof(password);
    // 使用 aes_encrypt_with_padding 进行加密，该函数将处理填充
    aes_encrypt_with_padding(password, encrypted_len, encrypted_output, key, iv);

    xVerifySemaphore = xSemaphoreCreateBinary();   // 创建验证密码的信号量
    xWordUsingSemaphore = xSemaphoreCreateMutex(); // 创建互斥信号量，用于保护共享资源iv和加密的密码
    xEncryptSemaphore = xSemaphoreCreateBinary();  // 创建加密信号量
    xChangeSemaphore = xSemaphoreCreateBinary();
    xConfirmSemaphore = xSemaphoreCreateBinary();
    // 创建定时器
    xTimer = xTimerCreate("Timer", pdMS_TO_TICKS(60000), pdTRUE, (void *)0, vTimerCallback);
    passwordQueue = xQueueCreate(10, sizeof(char[6])); // 创建密码队列

    if (xTimer != NULL)
    {
        xTimerStart(xTimer, 0);
    }
    xTaskCreate((TaskFunction_t)start_task,          /* 任务函数 */
                (const char *)"start_task",          /* 任务名称 */
                (uint16_t)START_STK_SIZE,            /* 栈大小 */
                (void *)NULL,                        /* 传递给任务函数的参数 */
                (UBaseType_t)START_TASK_PRIO,        /* 任务优先级 */
                (TaskHandle_t *)&StartTask_Handler); /* 任务句柄 */
    vTaskStartScheduler();                           /* 启动任务调度器 */
}

/**
 * @brief       start_task
 * @param       pvParameters : 传递给任务的参数(未使用)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); /* 进入临界区 */

    /* 创建LVGL任务 */
    xTaskCreate((TaskFunction_t)lv_demo_task,
                (const char *)"lv_demo_task",
                (uint16_t)LV_DEMO_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LV_DEMO_TASK_PRIO,
                (TaskHandle_t *)&LV_DEMOTask_Handler);

    /*UI数据更新任务 */
    xTaskCreate((TaskFunction_t)UITask,
                (const char *)"UITask",
                (uint16_t)UI_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)UI_TASK_PRIO,
                (TaskHandle_t *)&UITask_Handler);

    /* ESP8266任务 */
    xTaskCreate((TaskFunction_t)ESP8266_task,
                (const char *)"ESP8266_task",
                (uint16_t)ESP8266_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)ESP8266_TASK_PRIO,
                (TaskHandle_t *)&ESP8266Task_Handler);

    /* 密码验证任务 */
    xTaskCreate((TaskFunction_t)Verify_Task,
                (const char *)"Verify_Task",
                (uint16_t)VERIFY_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)VERIFY_TASK_PRIO,
                (TaskHandle_t *)&VerifyTask_Handler);

    /* 门锁状态任务 */
    xTaskCreate((TaskFunction_t)LockStatus_Task,
                (const char *)"LockStatus_Task",
                (uint16_t)LOCKSTATUS_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LOCKSTATUS_TASK_PRIO,
                (TaskHandle_t *)&LockStatusTask_Handler);

    xTaskCreate((TaskFunction_t)Encrypt_Task,
                (const char *)"Encrypt_Task",
                (uint16_t)ENCRYPT_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)ENCRYPT_TASK_PRIO,
                (TaskHandle_t *)&EncryptTask_Handler);

    xTaskCreate((TaskFunction_t)ChangePWSD_Task,
                (const char *)"ChangePWSD_Task",
                (uint16_t)CHANGEPWSD_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)CHANGEPWSD_TASK_PRIO,
                (TaskHandle_t *)&ChangePWSDTask_Handler);
    taskEXIT_CRITICAL();            /* 退出临界区 */
    vTaskDelete(StartTask_Handler); /* 删除初始任务 */
}

/**
 * @brief       LVGL任务函数
 * @param       pvParameters : 传递给任务的参数(未使用)
 * @retval      无
 */
void lv_demo_task(void *pvParameters)
{

    lv_mainstart(); /* LVGL演示 */
    while (1)
    {

        lv_timer_handler(); /* LVGL定时器 */
        vTaskDelay(10);
    }
}

/**
 * @brief   更新LVGL显示数据任务
 *
 * @param pvParameters
 */
void UITask(void *pvParameters)
{

    for (;;)
    {
        if (xSemaphoreTake(xChangeSemaphore, portMAX_DELAY) == pdTRUE)
        {
            password[0] = '\0'; // 清空密码
            lv_label_set_text(label_input, password);
            lv_label_set_text(label_user, "Change Word"); /* 设置文本 */
            changeStatus = 1;
        }
    }
}

/**
 * @brief   ESP8266任务
 *
 * @param pvParameters
 */
void ESP8266_task(void *pvParameters)
{
    while (1)
    {

        vTaskDelay(100);
    }
}
// 密码验证任务
void Verify_Task(void *pvParameters)
{
    uint8_t received_password[7];
    while (1)
    {
        if (xSemaphoreTake(xVerifySemaphore, portMAX_DELAY) == pdTRUE)
        {
            if (xQueueReceive(passwordQueue, received_password, portMAX_DELAY) == pdTRUE)
            {
            }

            if (xSemaphoreTake(xWordUsingSemaphore, portMAX_DELAY) == pdTRUE)
            {
                // 互斥信号量收到，执行密码验证操作
                if (verify_password(encrypted_output, 16, received_password, key, iv))
                {
                    // 密码验证成功
                    LED1(0);
                    SG90_Rotate90();
                    vTaskDelay(500);
                    // 信号量收到，执行密码验证操作

                    printf("Password verified\n");
                    LED1(1);
                }
                else
                {
                    // 密码验证失败
                    LED0(0);
                    password[0] = '\0'; // 清空密码
                    lv_label_set_text(label_input, password);
                    vTaskDelay(500);
                    LED0(1);
                }
                xSemaphoreGive(xWordUsingSemaphore);
            }
        }
    }
}
// 门锁状态任务
void LockStatus_Task(void *pvParameters)
{
    float distance;
    while (1)
    {
        distance = HCSR04_Measure_Distance();
        if (distance >= 10)
        {
            printf("门锁已打开\n");
            Ali_LockStatus(1);
        }
        else
        {
            printf("门锁已关闭\n");
            Ali_LockStatus(0);
        }
        vTaskDelay(10000);
    }
}
// 加密任务
void Encrypt_Task(void *pvParameters)
{

    while (1)
    { // 等待信号量
        if (xSemaphoreTake(xEncryptSemaphore, portMAX_DELAY) == pdTRUE)
        {
            if (xSemaphoreTake(xWordUsingSemaphore, portMAX_DELAY) == pdTRUE)
            {
                uint8_t password[6] = {'1', '2', '3', '4', '5', '6'};
                size_t password_len = sizeof(password);
                aes_encrypt_with_padding(password, password_len, encrypted_output, key, iv);
                Generate_AES_Key(key); // 生成AES密钥
                xSemaphoreGive(xWordUsingSemaphore);
            }
        }
    }
}
void StorageWord(void)
{
    // 保存加密后的密码到SD卡
    save_encrypted_password_to_sd(encrypted_output, 16);
}

void changePWSD(uint8_t *password, uint8_t *encrypted_PWSD)
{

    uint8_t start_key[16] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F, 0x70, 0x81,
                             0x92, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0x07}; // 初始密钥
    uint8_t start_iv[16] = {0x6A, 0x7B, 0x8C, 0x9D, 0x0E, 0x1F, 0x20, 0x31,
                            0x42, 0xA3, 0xB4, 0xC5, 0xD6, 0xE7, 0xF8, 0x0};     // 初始IV
                                                                                // 获取修改后的密码
                                                                                // 加密密码
    aes_encrypt_with_padding(password, 6, encrypted_PWSD, start_key, start_iv); // 加密密码
}

void ChangePWSD_Task(void *pvParameters)
{
    uint8_t received_password[7];
    uint8_t encrypted_PWSD[16 * 2];

    while (1)
    {
        if (xSemaphoreTake(xConfirmSemaphore, portMAX_DELAY) == pdTRUE)
        {
            if ((xQueueReceive(passwordQueue, received_password, portMAX_DELAY) == pdTRUE))
            {
                // changePWSD(received_password, encrypted_PWSD);
                // save_encrypted_password_to_sd(encrypted_PWSD, 16); // 存储加密后的密码到SD卡
                password[0] = '\0'; // 清空密码
                lv_label_set_text(label_input, password);
                lv_label_set_text(label_user, "USER"); /* 设置文本 */
                changeStatus = 0;
            }
        }
    }
}
