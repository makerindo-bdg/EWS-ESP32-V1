#include "header/statusHandler.h"

void statusHandler::setLastMode(int lastMode)
{
    this->lastMode = lastMode;
}

int statusHandler::getLastMode()
{
    return this->lastMode;
}

void statusHandler::setLastTime(unsigned long lastTime)
{
    this->lastTime = lastTime;
}

String statusHandler::modeToString(int mode)
{
    switch (mode)
    {
    case 0:
        return "MODE_STANBY";
        break;
    case 1:
        return "MODE_SIAGA3";
        break;
    case 2:
        return "MODE_SIAGA2";
        break;
    case 3:
        return "MODE_SIAGA1";
        break;
    }

    return "";
}

unsigned long statusHandler::getLastTime()
{
    return this->lastTime;
}

void statusHandler::setCurrentTime(unsigned long currentTime)
{
    this->currentTime = currentTime;
}

unsigned long statusHandler::getCurrentTime()
{
    return this->currentTime;
}

void statusHandler::setOperate(bool isOperate)
{
    this->isOperate = isOperate;
}

bool statusHandler::getOperate()
{
    return this->isOperate;
}

void statusHandler::setOverrideMode(int mode)
{
    this->overrideMode = mode;
}

int statusHandler::getOverrideMode()
{
    return this->overrideMode;
}

void statusHandler::setStatusChanged(bool isStatusChanged)
{
    this->isStatusChanged = isStatusChanged;
}

bool statusHandler::getStatusChanged()
{
    return this->isStatusChanged;
}

statusHandler::statusHandler(/* args */)
{
    this->mode = 0;
    this->isBuzzerOn = false;
}

void statusHandler::setMode(int mode)
{
    this->lastMode = this->mode;
    this->mode = mode;
}

int statusHandler::getMode()
{
    return this->mode;
}

void statusHandler::setBuzzer(bool isBuzzerOn)
{
    this->isBuzzerOn = isBuzzerOn;
}

bool statusHandler::getBuzzer()
{
    return this->isBuzzerOn;
}

statusHandler::~statusHandler()
{
}
