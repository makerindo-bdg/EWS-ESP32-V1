#include <dev_config.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

class Storage
{
public:
  Storage();
  void startEEPROM(int size);
  void saveConfig(JsonDocument doc);
  JsonDocument loadConfig();
  void setConfig(String ssid, String password, String mqtt_server, String mqtt_port, String mqtt_user, String mqtt_password, String mqtt_topic);
  String getSSID();
  String getPassword();
  String getMQTTServer();
  String getMQTTPort();
  String getMQTTUser();
  String getMQTTPassword();
  String getMQTTTopic();
};
