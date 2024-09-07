#if !defined(__DHT22__H)
#define __DHT22__H
void DHT22_Init(uint32_t Mode);//初始化
uint8_t DHT22_Read();//读取数据
uint8_t DHT22_GetData();//获取数据

#endif // __DHT22__H

