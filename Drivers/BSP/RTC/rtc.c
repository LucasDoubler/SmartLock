/**
 * @file rtc.c
 * @brief RTC驱动文件
 *
 * 该文件包含了RTC模块的初始化函数和相关配置函数。
 */

/**
 * @brief RTC模块初始化函数
 *
 * 该函数用于配置LSE作为RTC时钟源，并初始化RTC模块。
 */
void rtc_init(void);
#include "./BSP/RTC/rtc.h"
#include "./SYSTEM/usart/usart.h"
#include "FreeRTOS.h"
RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
/**
 * @brief 初始化RTC模块
 *
 * 该函数用于配置LSE作为RTC的时钟源，并初始化RTC模块。
 *
 * @note 在调用该函数之前，需要确保RCC已经配置好LSE。
 *
 * @note 该函数会启用RTC时钟，并根据给定的参数初始化RTC模块。
 *
 * @note 该函数会调用Error_Handler()函数来处理错误情况。
 */
void rtc_init(void)
{
    /* 配置LSE作为RTC时钟源 */
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /* 配置LSE */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;


    /* 选择LSE作为RTC时钟源 */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

    /* 启用RTC时钟 */
    __HAL_RCC_RTC_ENABLE();

    /* 初始化RTC */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
}
/**
 * @brief 获取当前时间
 *
 * 该函数用于获取当前RTC的时间，并将其转换为字符串。
 *
 * @param timeStr 用于存储时间字符串的缓冲区，推荐使用长度为20的数组。
 */
// void rtc_GetTime(char *timeStr)
// {

//     HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
//     HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
//     snprintf(timeStr, 24, "%u:%u:%u %u-%u-%u \n",
//              sTime.Hours, sTime.Minutes, sTime.Seconds,
//              sDate.Date, sDate.Month, 2000 + sDate.Year);
// }
void rtc_GetTime(TimeData_t *sensorData)
{
    // 获取时间和日期
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    // 将时间和日期存储到SensorData_t结构体中
    sensorData->hour = sTime.Hours;
    sensorData->minute = sTime.Minutes;
    sensorData->second = sTime.Seconds;
    sensorData->day = sDate.Date;
    sensorData->month = sDate.Month;
    sensorData->year = 2000 + sDate.Year;
}
void SetRTC(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    // 设置时间为 00:00:00
    sTime.Hours = 20;
    sTime.Minutes = 48;
    sTime.Seconds = 30;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    // if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    // {
    //     // 错误处理
    //     printf("RTC set time failed.\r\n");
    // }
    // else
    // {
    //     printf("RTC set time success.\r\n");
    // }

    // 设置日期为 2024年8月17日
    sDate.WeekDay = RTC_WEEKDAY_SATURDAY; // 根据具体日期填写
    sDate.Month = RTC_MONTH_AUGUST;
    sDate.Date = 17;
    sDate.Year = 24; // 表示2024年, HAL库中年份只需填写最后两位数字

    // if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    // {
    //     // 错误处理
    //     printf("RTC set date failed.\r\n");
    // }
    // else
    // {
    //     printf("RTC set date success.\r\n");
    // }
}
