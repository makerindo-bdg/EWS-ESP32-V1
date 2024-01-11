#include "dev_config.h"
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <HardwareSerial.h>
// #include <>
class Connection
{
public:
  Connection();

  String getLastMessageMQTT();
  String getLastMessageSerial();
  void startWiFiManager(String APName, String APPassword, int Mode, int timeout);
  void connectMQTT(String server, int port, String id, String user = "", String password = "");
  void connectSerial(int baudrate, int rx, int tx);
  static void mqttCallback(char *topic, byte *payload, unsigned int length);
  void mqttSubscribe(String topic);
  void mqttPublish(String topic, String msg);
  void mqttReconnect();
  void connectionLoop();
  void connectionLoopMQTT();
  void connectionLoopSerial();
  void reconnectWiFi();
  void disconnect();
  bool isConnected();
  String getWifiSSID();
  String getWifiPassword();
  void send(String message);
  void sendSerial(String message);
  String receive();
  static String dataMsg;

private:
  bool isMsgFinished;
  bool isWMStarted = false;
  HardwareSerial Serial_Con;
  String dataSerial;
  bool isDataSerialFullfilled = false;

  String saved_wifi_ssid;
  String saved_wifi_password;
  String last_topic_subscribed;
  String last_topic_published;
  String id_mqtt;
  String mqttServer;
  String AP_Name;
  String AP_Password;
  bool wifi_connected;
  bool mqtt_connected;
  WiFiManager wm;
  WiFiClient espClient;
  PubSubClient client;
};