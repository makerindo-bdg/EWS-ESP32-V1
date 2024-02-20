#include "OverTheAir.h"

OverTheAir::OverTheAir(/* args */)
{
}

OverTheAir::~OverTheAir()
{
}

bool OverTheAir::update(String server, int port, String endpoint, bool secure)
{
    httpUpdate.rebootOnUpdate(false); // remove automatic update

    client.setInsecure(); // remove ssl certificate

    Serial.println(F("Update start now!"));

    // t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://192.168.1.125:3000/firmware/httpUpdateNew.bin");
    // Or:
    t_httpUpdate_return ret;
    if (secure)
    {
        ret = httpUpdate.update(client, server, port, endpoint);
    }
    else
    {
        ret = httpUpdate.update(client_not_secure, server, port, endpoint);
    }

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        delay(1000); // Wait a second and restart
        return true;
        break;
    }

    return false;
}