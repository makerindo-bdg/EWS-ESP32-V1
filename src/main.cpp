#include <Arduino.h>
#include <TaskScheduler.h>
#include <OneButton.h>
#include <ArduinoQueue.h>
#include <Connection.h>
#include <Storage.h>
#include <Status.h>
#include <Output.h>
#include <dev_config.h>
#include <OverTheAir.h>

#define MS_TO_S_FACTOR 1000

void t1Callback();
void callbackRestartModem();
void t2Callback();
Task t1(20000, TASK_FOREVER, &t1Callback);
Task t2(7000, TASK_FOREVER, &t2Callback);
Task restartModem(3600UL * MS_TO_S_FACTOR, TASK_FOREVER, &callbackRestartModem);

bool isInternetConnected = false;
bool isInternetConnectedBefore = false;

String lastDate = "";

bool modemDown = false;

struct tm main_ptm;

int connectionCounter = 0;

void getTime();

typedef struct
{
  int timer;
  bool isOn;
} timerConfig;

Scheduler runner;
Status status_h;

ArduinoQueue<timerConfig> instructions(20);

Output buzzer(2);
Output modem(16);

OverTheAir ota;
Connection con_h;
Storage storage_h;
JsonDocument doc_terima;
JsonDocument doc_kirim;

OneButton button(0, true);

timerConfig ins;
const long utcOffsetInSeconds = 25200;

void restartWiFiManager()
{
  con_h.startWiFiManager("WM_" + (String)DEVICE_ID, "ews12345", WIFI_SET_ONDEMAND, 120);
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
#endif

  con_h.connectSerial(9600, 32, 33);

  con_h.startWiFiManually(SSID_MODEM, PASSWORD);
  // con_h.startWiFiManager("WM_" + (String)DEVICE_ID, "ews12345", WIFI_SET_AUTOCONNECT, 120);
  con_h.reconnectWiFi();

  con_h.connectMQTT(MQTT_SERVER, 1883, MQTT_ID);
  con_h.mqttSubscribe(MQTT_TOPIC_SUBSCRIBE);
  runner.init();
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(restartModem);
  t1.enable();
  t2.enable();

  button.attachClick(restartWiFiManager);

  configTime(utcOffsetInSeconds, 0, "id.pool.ntp.org");
}

void loop()
{


  if (isInternetConnected != isInternetConnectedBefore)
  {
    /* code */
    if (isInternetConnected)
    {
      restartModem.disable();

      con_h.sendSerial("0,0,1,*");
    }
    else
    {
      restartModem.enable();
      con_h.sendSerial("0,0,0,*");
    }
    isInternetConnectedBefore = isInternetConnected;
  }

  String msgMQTT = con_h.getLastMessageMQTT();
  if (msgMQTT != "")
  {
#ifdef DEBUG
    Serial.println(msgMQTT);
#endif

    deserializeJson(doc_terima, msgMQTT);

    int mode = 100;
    if (!doc_terima["id"].isNull())
    {
      if (doc_terima["id"].as<String>() == DEVICE_ID)
      {
        Serial.println("ID Match");
        con_h.mqttPublish("ews/debug", "Update Starting: " + (String)DEVICE_ID + " On Primary Server");
        // https://bpbdbekasikab.online/
        // bool result = ota.update("13.213.30.106", 80, "/update/firmware?id=" + (String)DEVICE_ID);
        bool result = ota.update("bpbdbekasikab.online", 443, (String) "/firmware/download?serial_number=" + (String)DEVICE_ID, true);
        if (result)
        {
          Serial.println("Update Success");
          con_h.reconnectWiFi();
          delay(100);
          con_h.connectMQTT(MQTT_SERVER, 1883, MQTT_ID + (String) "a");
          delay(100);
          con_h.mqttPublish("ews/debug", "Update Success: " + (String)DEVICE_ID + " On Primary Server");
          delay(100);
          ESP.restart();
        }
        else
        {
          Serial.println("Update Failed");
          con_h.mqttReconnect();
          delay(100);
          con_h.mqttPublish("ews/debug", "Update Failed:" + (String)DEVICE_ID + " Trying Secondary Server");

          result = ota.update("bitanic.id", 443, (String) "/update/firmware?id=" + (String)DEVICE_ID, true);
          if (result)
          {
            Serial.println("Update Success");
            delay(100);
            con_h.reconnectWiFi();
            delay(1000);
            con_h.connectMQTT(MQTT_SERVER, 1883, MQTT_ID + (String) "b");
            delay(100);
            con_h.mqttPublish("ews/debug", "Update Success: " + (String)DEVICE_ID + " On Secondary Server");
            delay(100);
            ESP.restart();
          }
          else
          {
            Serial.println("Update Failed");
            con_h.mqttReconnect();
            delay(100);
            con_h.mqttPublish("ews/debug", "Update Failed:" + (String)DEVICE_ID + " On Secondary Server");
          }
        }
      }
    }
    else if (!doc_terima["override_mode"].isNull())
    {
      if (status_h.getOverrideMode() != doc_terima["override_mode"].as<int>())
      {
        status_h.setOverrideMode(doc_terima["override_mode"].as<int>());
      }

      if (status_h.getOverrideMode() != 100)
      {
        mode = doc_terima["override_mode"].as<int>();
        status_h.setOverrideMode(mode);
      }
      else
      {
        mode = doc_terima["mode"].as<int>();
      }

      if (status_h.getMode() != mode)
      {
        status_h.setMode(mode);

        while (!instructions.isEmpty())
        {
          instructions.dequeue();
        }

        if (mode == 0)
        {
#ifdef DEBUG
          Serial.println("Mode Stanby");
#endif
          con_h.sendSerial("0,0,1,*");
          /* code */
        }
        else if (mode == 1)
        {
#ifdef DEBUG
          Serial.println("Mode Siaga3");
#endif
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({5000, true});

          instructions.enqueue({2000, false});

          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({5000, true});

          instructions.enqueue({2000, false});

          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({5000, true});
        }
        else if (mode == 2)
        {
#ifdef DEBUG
          Serial.println("Mode Siaga2");
#endif
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({5000, true});

          instructions.enqueue({2000, false});

          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({5000, true});

          instructions.enqueue({2000, false});

          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({1000, true});
          instructions.enqueue({500, false});
          instructions.enqueue({5000, true});
        }
        else if (mode == 3)
        {
#ifdef DEBUG
          Serial.println("Mode Siaga1");
#endif
          instructions.enqueue({40000, true});
        }

        // instructions.enqueue();
      }
    }
  }

  if (!instructions.isEmpty() && !status_h.getOperate())
  {
    ins = instructions.dequeue();
#ifdef DEBUG
    Serial.println("Timer: " + (String)ins.timer);
    Serial.println("isOn: " + (String)ins.isOn);
#endif

    if (ins.isOn)
    {
      // TODO ON Lampu, Buzzer
      buzzer.on();
#ifdef DEBUG
      Serial.println("Buzzer On");
#endif
      con_h.sendSerial((String)status_h.getMode() + (String) ",1,1,*");
    }

    status_h.setLastTime(millis());
    status_h.setOperate(true);
  }

  if (status_h.getOperate())
  {
    if (millis() - status_h.getLastTime() >= ins.timer)
    {
      // TODO OFF Lampu, Buzzer
      if (ins.isOn)
      {
        buzzer.off();
// TODO ON Lampu, Buzzer
#ifdef DEBUG
        Serial.println("Buzzer Off");
#endif
        con_h.sendSerial((String)status_h.getMode() + (String) ",0,1,*");
      }

      status_h.setOperate(false);
    }
  }

  String msgSerial = con_h.getLastMessageSerial();
  if (msgSerial != "")
  {
    String dataStr[8];
#ifdef DEBUG
    Serial.print(msgSerial);
#endif

    int index1 = msgSerial.indexOf(',');
    int index2 = msgSerial.indexOf(',', index1 + 1);
    int index3 = msgSerial.indexOf(',', index2 + 1);
    int index4 = msgSerial.indexOf(',', index3 + 1);
    int index5 = msgSerial.indexOf(',', index4 + 1);
    int index6 = msgSerial.indexOf(',', index5 + 1);
    int index7 = msgSerial.indexOf(',', index6 + 1);
    int index8 = msgSerial.indexOf(',', index7 + 1);

    dataStr[0] = msgSerial.substring(0, index1);
    dataStr[1] = msgSerial.substring(index1 + 1, index2);
    dataStr[2] = msgSerial.substring(index2 + 1, index3);
    dataStr[3] = msgSerial.substring(index3 + 1, index4);
    dataStr[4] = msgSerial.substring(index4 + 1, index5);
    dataStr[5] = msgSerial.substring(index5 + 1, index6);
    dataStr[6] = msgSerial.substring(index6 + 1, index7);
    dataStr[7] = msgSerial.substring(index7 + 1, index8);

    getTime();

    int monthDay = main_ptm.tm_mday;

    int currentMonth = main_ptm.tm_mon + 1;

    int currentYear = main_ptm.tm_year + 1900;

    int hour = main_ptm.tm_hour;

    int minute = main_ptm.tm_min;

    int sec = main_ptm.tm_sec;

    doc_kirim.clear();

    doc_kirim["serial_number"] = DEVICE_ID;
    doc_kirim["date_time"] = String(currentYear) + "-" + (currentMonth < 10 ? "0" + (String)currentMonth : (String)currentMonth) + "-" + (monthDay < 10 ? "0" + (String)monthDay : (String)monthDay) + " " + (hour < 10 ? "0" + (String)hour : (String)hour) + ":" + (minute < 10 ? "0" + (String)minute : (String)minute) + ":" + (sec < 10 ? "0" + (String)sec : (String)sec);
    // doc_kirim["date_time"] = "2021-01-01 00:00:00";
    doc_kirim["firmware_version"] = DEVICE_FIRMWARE_VERSION;
    doc_kirim["data_sensor"]["voltage"] = dataStr[0].toFloat();
    doc_kirim["data_sensor"]["arus"] = dataStr[1].toFloat();
    doc_kirim["data_sensor"]["battery_consumtion"] = dataStr[2].toFloat();
    doc_kirim["data_sensor"]["intensistas_cahaya"] = dataStr[3].toFloat();
    doc_kirim["status_sistem"]["current_ticking_time"] = dataStr[4].toInt();
    doc_kirim["status_sistem"]["current_sendtime_period"] = dataStr[5].toInt();
    doc_kirim["status_sistem"]["current_buzzer_state"] = dataStr[6].toInt();
    doc_kirim["status_sistem"]["current_mode"] = dataStr[7].toInt();

    String temp;
    serializeJson(doc_kirim, temp);

    con_h.mqttPublish(MQTT_TOPIC_PUBLISH, temp.c_str());
    buzzer.on();
    delay(100);
    buzzer.off();
  }

  if (connectionCounter >= 5)
  {

    connectionCounter = 0;
    isInternetConnected = false;
  }

  con_h.connectionLoopMQTT();
  button.tick();
  runner.execute();
  con_h.connectionLoop();
  con_h.connectionLoopSerial();
}

void callbackRestartModem()
{
  Serial.println("Restart Modem");
  modem.on();
  delay(5000);
  modem.off();
  // con_h.sendSerial("REQ,*");
}

void t2Callback()
{
  Serial.println("Check Connection");
  con_h.reconnectWiFi();

  if (!con_h.isConnected())
  {
    if (millis() > 300 * MS_TO_S_FACTOR)
    {
      connectionCounter++;
      Serial.print("RunCounter Modem");
      Serial.println(restartModem.getRunCounter());

      /* code */
    }
  }
  else
  {
    isInternetConnected = true;
    connectionCounter = 0;
  }

  // con_h.sendSerial("REQ,*");
}

void t1Callback()
{
  Serial.println("Hello World");
  con_h.sendSerial("REQ,*");
}

void getTime()
{

  if (!getLocalTime(&main_ptm))
  {
#ifdef DEBUG
    Serial.println("Failed to obtain time");
#endif
    return;
  }
  // String test = String("%Y-%m-%d", &main_main_ptm);
}