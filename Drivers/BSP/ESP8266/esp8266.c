#include "stm32h7xx_hal.h"
#include "./SYSTEM/delay/delay.h"
#include "SYSTEM/usart/usart.h"
#include "BSP/ESP8266/esp8266.h"
#include "cJSON/cJSON.h"

char Ali_Config[] = "AT+MQTTUSERCFG=0,1,\"Lock_Notice|securemode=2\\,signmethod=hmacsha1\\,timestamp=1724923143856|\","
                    "\"Lock_Notice&k1otf4mdEkF\",\"BA090F0492BE86DB73D03D05777FA4E82884CB36\",0,0,\"\"\r\n";
/*阿里云登录命令*/
char Ali_Signup[] = "AT+MQTTCONN=0,\"k1otf4mdEkF.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,0\r\n";
/*阿里云发布命令*/
char Ali_SendData[] = "AT+MQTTPUB=0,\"/sys/k1otf4mdEkF/Lock_Notice/thing/event/property/post\","
                      "\"{\\\"params\\\":{\\\"LockState\\\":1,\\\"DeviceStatus\\\":1}\\,\\\"version\\\":\\\"1.0.0\\\"}\",0,0\r\n";
void ESP8266_Init(void)
{
    // 初始化 ESP8266
    UART3_Transmit("AT+CWMODE=1\r\n");                      // 设置为Station模式
    delay_ms(500);                                          // 等待ESP8266响应
    UART3_Transmit("AT+CWJAP=\"MyHost\",\"12345678\"\r\n"); // 连接热点
    delay_ms(2000);                                         // 等待ESP8266响应
}
void Aliyun_Config(void)
{
    // 配置阿里云
    UART3_Transmit(Ali_Config); // 配置阿里云
    delay_ms(2000);             // 等待ESP8266响应
    UART3_Transmit(Ali_Signup); // 登录阿里云
    delay_ms(2000);
}
void Ali_LockStatus(uint8_t Status)
{
    char at_command[132]; // 确保缓冲区足够大

    snprintf(at_command, 132,
             MQTT_LockState, Status);
    UART3_Transmit(at_command);
}
void Ali_DeviceStatus(uint8_t Status)
{
    char at_command[135]; // 确保缓冲区足够大
    snprintf(at_command, 135,
             MQTT_DeviceStatus, Status);
    UART3_Transmit(at_command);
}
uint8_t is_aliyun_message(const char *received_message)
{
    // 使用 strncmp 比较接收的数据是否以阿里云的前缀开头
    if (strncmp(received_message, ALIYUN_PREFIX, strlen(ALIYUN_PREFIX)) == 0)
    {
        return 1; // 是阿里云的消息
    }
    else
    {
        return 0; // 不是阿里云的消息
    }
}

char json_data[MAX_JSON_LENGTH];

void extract_json_from_mqtt_message(const char *message)
{
    // 使用显式类型转换将 strchr 返回值转换为 char*
    char *json_start = (char *)strchr(message, '{');

    if (json_start != NULL)
    {
        // 将 JSON 数据复制到 json_data 数组中
        strncpy(json_data, json_start, MAX_JSON_LENGTH - 1);
        json_data[MAX_JSON_LENGTH - 1] = '\0'; // 确保字符串以 null 结尾

        printf("Extracted JSON: %s\n", json_data); // 调试输出提取的 JSON
    }
    else
    {
        printf("JSON not found in message\n");
    }
}
int changeSem = 0;
uint8_t parse_mqtt_message(const char *message)
{
    cJSON *json = cJSON_Parse(message);
    if (json == NULL)
    {
        printf("JSON Parsing Error\n");
        return 0;
    }

    // 提取 "params" 对象
    cJSON *params = cJSON_GetObjectItem(json, "params");
    if (params == NULL)
    {
        printf("Could not find params\n");
        cJSON_Delete(json);
        return 0;
    }

    // 提取 "ChangeNotice" 值
    cJSON *change_notice = cJSON_GetObjectItem(params, "ChangeNotice");
    if (change_notice == NULL)
    {
        printf("Could not find ChangeNotice\n");
        cJSON_Delete(json);
        return 0;
    }

    // 检查 ChangeNotice 值并设置 changeSem 变量
    if (cJSON_IsNumber(change_notice))
    {
        return 1;
    }

    cJSON_Delete(json); // 清理内存
}
