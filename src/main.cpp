#include <Arduino.h>
#include <TaskScheduler.h>
#include <ArduinoQueue.h>
#include "header/connection.h"
#include "header/storageHandler.h"
#include "header/statusHandler.h"
#include "header/outputHandler.h"

void t1Callback();
Task t1(5000, TASK_FOREVER, &t1Callback);

typedef struct
{
  int timer;
  bool isOn;
} timerConfig;

Scheduler runner;
statusHandler status_h;

ArduinoQueue<timerConfig> instructions(20);

outputHandler buzzer(2);

Connection con_h;
StorageHandler storage_h;
JsonDocument doc_terima;
JsonDocument doc_kirim;

timerConfig ins;

void setup()
{
  Serial.begin(115200);
  con_h.connectSerial(9600, 33, 32);

  con_h.startWiFiManager("SETTING", "admin1234", WIFI_SET_AUTOCONNECT, 120);
  con_h.connectMQTT(MQTT_SERVER, 1883, MQTT_ID);
  con_h.mqttSubscribe(MQTT_TOPIC_SUBSCRIBE);
  runner.init();
  runner.addTask(t1);
  t1.enable();
}

void loop()
{
  String msgMQTT = con_h.getLastMessageMQTT();
  if (msgMQTT != "")
  {
    Serial.println(msgMQTT);

    deserializeJson(doc_terima, msgMQTT);

    int mode = 100;
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
        Serial.println("Mode Stanby");
        con_h.sendSerial("0,0,1,*");
        /* code */
      }
      else if (mode == 1)
      {
        Serial.println("Mode Siaga3");
        instructions.enqueue({1000, true});
        instructions.enqueue({1000, false});
        instructions.enqueue({1000, true});
        instructions.enqueue({1000, false});
        instructions.enqueue({1000, true});
        instructions.enqueue({1000, false});
      }
      else if (mode == 2)
      {
        Serial.println("Mode Siaga2");
        instructions.enqueue({1000, true});
        instructions.enqueue({1000, false});
        instructions.enqueue({1000, true});
        instructions.enqueue({1000, false});
      }
      else if (mode == 3)
      {
        Serial.println("Mode Siaga1");
        instructions.enqueue({1000, true});
        instructions.enqueue({1000, false});
      }

      // instructions.enqueue();
    }
  }

  if (!instructions.isEmpty() && !status_h.getOperate())
  {
    ins = instructions.dequeue();
    Serial.println("Timer: " + (String)ins.timer);
    Serial.println("isOn: " + (String)ins.isOn);

    if (ins.isOn)
    {
      // TODO ON Lampu, Buzzer
      buzzer.on();
      Serial.println("Buzzer On");
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
        Serial.println("Buzzer Off");
        con_h.sendSerial((String)status_h.getMode() + (String) ",0,1,*");
      }

      status_h.setOperate(false);
    }
  }

  String msgSerial = con_h.getLastMessageSerial();
  if (msgSerial != "")
  {
    String dataStr[8];
    Serial.print(msgSerial);

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

    doc_kirim["serial_number"] = "PUB0002";
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
  }

  runner.execute();
  con_h.connectionLoopMQTT();
  con_h.connectionLoop();
  con_h.connectionLoopSerial();
}

void t1Callback()
{

  // Serial.println("Hello World");
  con_h.sendSerial("REQ,*");
}