#include <Arduino.h>

#define DEVICE_ID "PUB0003"
#define DEVICE_FIRMWARE_VERSION "1.0.0a"

#define WIFI_SET_AUTOCONNECT 0
#define WIFI_SET_ONDEMAND 1

#define JSON_SIZE 1024

#define MQTT_ID DEVICE_ID
#define MQTT_SERVER "193.168.195.119"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0003"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0003"

#define WIFI_AP_NAME "SETTING"
#define WIFI_AP_PASSWORD "admin1234"

#define DEBUG_WIFI_MANAGER 0

#define DEBUG true
