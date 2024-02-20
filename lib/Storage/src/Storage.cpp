#include "Storage.h"
#include <Status.h>

Storage::Storage()
{
}

void Storage::startEEPROM(int size)
{
    EEPROM.begin(size);
}

void Storage::saveConfig(JsonDocument doc)
{
    EEPROM.begin(4000);

    String test;
    serializeJson(doc, test);
    // Serial.println(test);

    EEPROM.writeString(2500, test);
    // Serial.println(test);

    EEPROM.commit();
    EEPROM.end();
    // Close the file
}

JsonDocument Storage::loadConfig()
{
    EEPROM.begin(4000);
    // Open file for reading
    JsonDocument doc;
    String test;
    test = EEPROM.readString(2500);
    // Serial.println(test);
    DeserializationError error = deserializeJson(doc, test);
    if (error)
    {
        
        Serial.println(F("Failed to read file, using default configuration"));
        doc["status"] = "error";
        return doc;
    }

    // Serial.println(doc["ssid"].as<String>());
    // EEPROM.end();
    return doc;
    // Extract each characters by one by one
    // while (file.available()) {
    //   Serial.print((char)file.read());
    // }
    // Serial.println();

    // Close the file
    EEPROM.end();
}