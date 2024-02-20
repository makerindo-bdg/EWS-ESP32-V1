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

void t1Callback();
void t2Callback();
Task t1(20000, TASK_FOREVER, &t1Callback);
Task t2(7000, TASK_FOREVER, &t2Callback);

typedef struct
{
  int timer;
  bool isOn;
} timerConfig;

Scheduler runner;
Status status_h;

ArduinoQueue<timerConfig> instructions(20);

Output buzzer(2);

OverTheAir ota;
Connection con_h;
Storage storage_h;
JsonDocument doc_terima;
JsonDocument doc_kirim;

OneButton button(0, true);

timerConfig ins;

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

  con_h.startWiFiManually();
  // con_h.startWiFiManager("WM_" + (String)DEVICE_ID, "ews12345", WIFI_SET_AUTOCONNECT, 120);
  con_h.reconnectWiFi();

  con_h.connectMQTT(MQTT_SERVER, 1883, MQTT_ID);
  con_h.mqttSubscribe(MQTT_TOPIC_SUBSCRIBE);
  runner.init();
  runner.addTask(t1);
  runner.addTask(t2);
  t1.enable();
  t2.enable();

  button.attachClick(restartWiFiManager);
}

void loop()
{
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
        // https://bpbdbekasikab.online/
        // bool result = ota.update("13.213.30.106", 80, "/update/firmware?id=" + (String)DEVICE_ID);
        bool result = ota.update("13.213.30.106", 3000, "/update/firmware", false);
        if (result)
        {
          Serial.println("Update Success");
          con_h.mqttReconnect();
          con_h.mqttPublish("ews/public_to_cloud_" + (String)DEVICE_ID + "/debug", "Update Success..");
          ESP.restart();
        }
        else
        {
          Serial.println("Update Failed");
          con_h.mqttReconnect();
          con_h.mqttPublish("ews/public_to_cloud_" + (String)DEVICE_ID + "/debug", "Update Failed..");
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

    doc_kirim["serial_number"] = DEVICE_ID;
    doc_kirim["date_time"] = "2021-01-01 00:00:00";
    doc_kirim["firmware_version"] = "V2.0.6";
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
  con_h.connectionLoopMQTT();
  button.tick();
  runner.execute();
  con_h.connectionLoop();
  con_h.connectionLoopSerial();
}

void t2Callback()
{
  Serial.println("Check Connection");
  con_h.reconnectWiFi();

  // con_h.sendSerial("REQ,*");
}

void t1Callback()
{
  Serial.println("Hello World");
  con_h.sendSerial("REQ,*");
}