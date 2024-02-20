#include "connection.h"
#include <EEPROM.h>

/* Core where the task should run */

String Connection::dataMsg = "";

String Connection::getLastMessageSerial()
{
  String temp;
  // this->dataSerial;
  if (this->dataSerial != "" && this->isMsgFinished)
  {
    temp = this->dataSerial;
    this->dataSerial = "";
    this->isMsgFinished = false;
    return temp;
  }
  else
  {
    return "";
  }

  return "";
}

void Connection::connectionLoopSerial()
{
  while (this->Serial_Con.available() > 0)
  {
    char c = this->Serial_Con.read();
    if (c == '*')
    {
      this->isMsgFinished = true;
    }
    this->dataSerial += c;
    // Serial.print(c);
    // this->dataSerial = this->Serial_Con.readString();
  }
}

void Connection::sendSerial(String message)
{
  this->Serial_Con.println(message);
}

void Connection::connectSerial(int baudrate, int rx, int tx)
{
  this->Serial_Con.begin(baudrate, SERIAL_8N1, rx, tx);
}

void Connection::startWiFiManually()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    // delay(1000);
    WiFi.mode(WIFI_STA);
    this->saved_wifi_ssid = this->getSavedWifiSSID();
    this->saved_wifi_password = this->getSavedWifiPassword();
    WiFi.begin(this->saved_wifi_ssid.c_str(), this->saved_wifi_password.c_str());
#ifdef DEBUG
    Serial.println("SSID: " + this->saved_wifi_ssid + " Password: " + this->saved_wifi_password);
#endif
    // delay(1000);
  }
}

void Connection::reconnectWiFi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    // delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->saved_wifi_ssid.c_str(), this->saved_wifi_password.c_str());
#ifdef DEBUG
    Serial.println("SSID: " + this->saved_wifi_ssid + " Password: " + this->saved_wifi_password);
#endif
    // delay(1000);
  }
}

Connection::Connection() : Serial_Con(2)
{
  this->wifi_connected = false;
  this->mqtt_connected = false;
}

String Connection::getSavedWifiSSID()
{
  EEPROM.begin(4000);
  this->saved_wifi_ssid = EEPROM.readString(2000);
  EEPROM.end();

  return this->saved_wifi_ssid;
}

String Connection::getSavedWifiPassword()
{
  EEPROM.begin(4000);
  this->saved_wifi_password = EEPROM.readString(2050);
  EEPROM.end();
  return this->saved_wifi_password;
}

String Connection::getLastMessageMQTT()
{
  String temp;
  // this->dataMsg;
  if (this->dataMsg != "")
  {
    temp = this->dataMsg;
    this->dataMsg = "";
    return temp;
  }
  else
  {
    return "";
  }

  return "";
}

bool Connection::isConnected()
{
  return this->wifi_connected && this->mqtt_connected;
}

void Connection::connectionLoopMQTT()
{
  if (!this->client.connected() && WiFi.status() == WL_CONNECTED)
  {
    this->mqttReconnect();
  }
}

void Connection::connectionLoop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    this->wifi_connected = true;
  }
  else
  {

    this->wifi_connected = false;
  }

  if (client.connected())
  {
    this->mqtt_connected = true;
  }
  else
  {
    this->mqtt_connected = false;
  }

  if (this->client.connected() && WiFi.status() == WL_CONNECTED)
  {
    this->client.loop();
  }

  // if (!this->isWMStarted)
  // {
  //   if (WiFi.status() != WL_CONNECTED)
  //   {
  //     delay(3000);
  //     wm.setConfigPortalTimeout(10);
  //     Serial.println("WiFi not connected, Reconnecting...");
  //     wm.autoConnect(this->AP_Name.c_str(), this->AP_Password.c_str());
  //   }
  // }
}

void Connection::disconnect()
{
  this->client.disconnect();
  this->wifi_connected = false;
  this->mqtt_connected = false;
}

void Connection::mqttCallback(char *topic, byte *payload, unsigned int length)
{
#ifdef DEBUG_MQTT
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  // for (int i = 0; i < length; i++)
  // {
  //   Serial.print((char)payload[i]);
  // }
  // Serial.println();
#endif

  for (int i = 0; i < length; i++)
  {
    dataMsg += (char)payload[i];
  }
}

void Connection::mqttReconnect()
{
  // Loop until we're reconnected

  // if (!this->client.connected() && WiFi.status() == WL_CONNECTED)
  // {
  //   this->mqttReconnect();
  // }

  if (!this->client.connected() && WiFi.status() == WL_CONNECTED)
  {
#ifdef DEBUG_MQTT
    Serial.print("Attempting MQTT connection...");
#endif
    // Attempt to connect
    this->client.setClient(this->espClient);
    this->client.setServer(MQTT_SERVER, 1883);
    this->client.setCallback(this->mqttCallback);
    this->client.setBufferSize(4096);
    if (this->client.connect(this->id_mqtt.c_str()))
    {
#ifdef DEBUG_MQTT
      Serial.println("connected");
#endif
      // Once connected, publish an announcement...
      // this->client.publish(this->last_topic_published.c_str(), "hello world");
      // ... and resubscribe
      this->client.subscribe(MQTT_TOPIC_SUBSCRIBE);
// client.flush();
#ifdef DEBUG
      Serial.println("Subscribed");
#endif
    }
    else
    {
#ifdef DEBUG_MQTT
      Serial.print("failed, rc=");
      Serial.print(this->client.state());
      Serial.println(" try again in 5 seconds");
#endif
      // Wait 5 seconds before retrying
      // delay(5000);
    }
  }
}

void Connection::mqttSubscribe(String topic)
{
  this->last_topic_subscribed = topic;
  this->client.subscribe(topic.c_str());
}

void Connection::mqttPublish(String topic, String msg)
{
  this->last_topic_published = topic;
  this->client.publish(topic.c_str(), msg.c_str());
}

void Connection::connectMQTT(String server, int port, String id, String user, String password)
{
  this->mqttServer = server;
  this->id_mqtt = id;
  this->client.setClient(this->espClient);
  this->client.setServer(mqttServer.c_str(), port);
  this->client.setCallback(this->mqttCallback);
  this->client.setBufferSize(4096);
  this->mqtt_connected = true;
  if (!this->client.connect(this->id_mqtt.c_str()))
  {
#ifdef DEBUG_MQTT
    Serial.println("MQTT not connected, Reconecting...");
#endif
    this->mqttReconnect();
  }
  // this->client.setCredentials(user.c_str(), password.c_str());
}

void Connection::startWiFiManager(String APName, String APPassword, int Mode, int timeout)
{
  bool res;

  this->AP_Name = APName;
  this->AP_Password = APPassword;

#ifdef DEBUG_WIFI_MANAGER
#ifdef DEBUG
  Serial.println("WiFiManager Started");
#endif
#endif
  wm.setConnectTimeout(10);
  wm.setConfigPortalTimeout(timeout);
  if (Mode == WIFI_SET_ONDEMAND)
  {
    // Settingan selalu menyalakan WiFi Manager Ketika Menyala
    res = wm.startConfigPortal(this->AP_Name.c_str(), this->AP_Password.c_str());
  }
  else if (Mode == WIFI_SET_AUTOCONNECT)
  {
    // Settingngan autoconnect kalau pernah konek ke wifi sebelumnya jadi ngga akan nyalain WiFi Manager
    res = wm.autoConnect(this->AP_Name.c_str(), this->AP_Password.c_str()); // password protected ap
  }

  if (!res)
  {
#ifdef DEBUG_WIFI_MANAGER
    Serial.println("Failed to connect");
#endif
    // ESP.restart();
  }
  else
  {
// if you get here you have connected to the WiFi
#ifdef DEBUG_WIFI_MANAGER
    this->saved_wifi_ssid = wm.getWiFiSSID();
    this->saved_wifi_password = wm.getWiFiPass();
    Serial.println("connected...yeey :)");
    EEPROM.begin(4000);
    EEPROM.writeString(2000, wm.getWiFiSSID());
    EEPROM.writeString(2050, wm.getWiFiPass());
    Serial.println("WiFi connected");
    Serial.println("Local IP");
    Serial.println(WiFi.localIP());
    EEPROM.commit();
    EEPROM.end();
#endif
    this->wifi_connected = true;
  }

  this->saved_wifi_ssid = this->getSavedWifiSSID();
  this->saved_wifi_password = this->getSavedWifiPassword();

#ifdef DEBUG_WIFI_MANAGER

#endif
}
