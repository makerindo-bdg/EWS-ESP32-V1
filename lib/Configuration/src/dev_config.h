#include <Arduino.h>

#define DEVICE_FIRMWARE_VERSION "1.5.12"
#define DEVICE_NUMBER 6
#define USE_PASSWORD_MQTT false
#define MQTT_SERVER "193.168.195.119"

#define WIFI_SET_AUTOCONNECT 0
#define WIFI_SET_ONDEMAND 1
#define JSON_SIZE 1024

#define MQTT_USER "mqttEws123"
#define MQTT_PASSWORD "earlyWarningSystemHebatBekasiMakerindo123456789"
#define WIFI_AP_NAME "SETTING"
#define WIFI_AP_PASSWORD "admin1234"

#if DEVICE_NUMBER == 1
#define DEVICE_ID "PUB0001"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0001"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0001"
#define SSID_MODEM "PUB0001"
#define PASSWORD "0057f266a03b4e7b59ec63fb3bb4c785"
#elif DEVICE_NUMBER == 2
#define DEVICE_ID "PUB0002"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0002"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0002"
#define SSID_MODEM "PUB0002"
#define PASSWORD "adde22265fc0f0a66aba25443878cda1"
#elif DEVICE_NUMBER == 3
#define DEVICE_ID "PUB0003"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0003"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0003"
#define SSID_MODEM "PUB0003"
#define PASSWORD "0f87c686f5eabbffebf226db215597e0"
#elif DEVICE_NUMBER == 4
#define DEVICE_ID "PUB0004"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0004"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0004"
#define SSID_MODEM "PUB0004"
#define PASSWORD "98d00d562d83b86cabc847529e650aeb"
#elif DEVICE_NUMBER == 5
#define DEVICE_ID "PUB0005"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0005"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0005"
#define SSID_MODEM "PUB0005"
#define PASSWORD "3ffca12380c46cbdc98f813fdc71bbae"
#elif DEVICE_NUMBER == 6
#define DEVICE_ID "PUB0006"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0006"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0006"
#define SSID_MODEM "PUB0006"
#define PASSWORD "df283958a669bd6a32b366dbcd3e1d7e"
#elif DEVICE_NUMBER == 7
#define DEVICE_ID "PUB0007"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0007"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0007"
#define SSID_MODEM "PUB0007"
#define PASSWORD "b491e01fae833f77fb9499dfc2b03ad9"
#elif DEVICE_NUMBER == 8
#define DEVICE_ID "PUB0008"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0008"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0008"
#define SSID_MODEM "PUB0008"
#define PASSWORD "a5eeee48221b22f5ea225c39a0e3f46c"
#elif DEVICE_NUMBER == 9
#define DEVICE_ID "PUB0009"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0009"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0009"
#define SSID_MODEM "PUB0009"
#define PASSWORD "af649647aef4342d677d3251ff5bb767"
#elif DEVICE_NUMBER == 10
#define DEVICE_ID "PUB0010"
#define MQTT_TOPIC_SUBSCRIBE "ews/cloud_to_public_PUB0010"
#define MQTT_TOPIC_PUBLISH "ews/public_to_cloud_PUB0010"
#define SSID_MODEM "PUB0010"
#define PASSWORD "53734b95f0390b2f58203c509bd5e769"
#endif

#define DEBUG_WIFI_MANAGER 0

#define DEBUG true

#define MQTT_ID DEVICE_ID