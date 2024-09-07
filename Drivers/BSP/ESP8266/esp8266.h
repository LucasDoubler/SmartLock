#if !defined(__ESP8266__H)
#define __ESP8266__H

#define MQTT_CLIENT_ID "Lock_Notice|securemode=2,signmethod=hmacsha1,timestamp=1724923143856|"
#define MQTT_USER_NAME "Lock_Notice&k1otf4mdEkF"
#define MQTT_PASSWORD "BA090F0492BE86DB73D03D05777FA4E82884CB36"
#define BROKER_ADDRESS "k1otf4mdEkF.iot-as-mqtt.cn-shanghai.aliyuncs.com"

#define PUB_TOPIC "/sys/k1otf4mdEkF/Lock_Notice/thing/event/property/post"
#define SUB_TOPIC "/sys/k1otf4mdEkF/Lock_Notice/thing/service/property/set"

#define ALIYUN_PREFIX "MQTTSUBRECV:0,\"/sys/k1otf4mdEkF/Lock_Notice/thing/service/property/set\""

#define LockState "{\\\"params\\\":{\\\"LockState\\\":%d}\\,\\\"version\\\":\\\"1.0.1\\\"}"
#define DeviceStatus "{\\\"params\\\":{\\\"DeviceStatus\\\":%d}\\,\\\"version\\\":\\\"1.0.1\\\"}"
#define MQTT_LockState "AT+MQTTPUB=0,\"" PUB_TOPIC "\",\"" LockState "\",0,0\r\n"
#define MQTT_DeviceStatus "AT+MQTTPUB=0,\"" PUB_TOPIC "\",\"" DeviceStatus "\",0,0\r\n"
#define MAX_JSON_LENGTH 256 // 定义 JSON 数据的最大长度

void ESP8266_Init(void);

void Aliyun_Config(void);

void Ali_LockStatus(uint8_t Status);
void Ali_DeviceStatus(uint8_t Status);
uint8_t parse_mqtt_message(const char *message);
void extract_json_from_mqtt_message(const char *message);

uint8_t is_aliyun_message(const char *received_message);

#endif // __ESP8266__H)
