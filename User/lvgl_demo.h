#include "./system/sys/sys.h"
#ifndef __LVGL_DEMO_H
#define __LVGL_DEMO_H
#define RECORD_COUNT 12

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} TimeData_t;
void lvgl_demo(void);  
#endif
