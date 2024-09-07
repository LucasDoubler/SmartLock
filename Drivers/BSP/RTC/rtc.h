#if !defined(__RTC__H)
#define __RTC__H
#include   "lvgl_demo.h"
// void rtc_GetTime(char *timeStr);
void rtc_GetTime(TimeData_t *sensorData);

void SetRTC(void);


#endif // __RTC__H


    
