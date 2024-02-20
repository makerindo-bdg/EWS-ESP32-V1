#include "Status.h"

void Status::setLastMode(int lastMode)
{
    this->lastMode = lastMode;
}

int Status::getLastMode()
{
    return this->lastMode;
}

void Status::setLastTime(unsigned long lastTime)
{
    this->lastTime = lastTime;
}

String Status::modeToString(int mode)
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

unsigned long Status::getLastTime()
{
    return this->lastTime;
}

void Status::setCurrentTime(unsigned long currentTime)
{
    this->currentTime = currentTime;
}

unsigned long Status::getCurrentTime()
{
    return this->currentTime;
}

void Status::setOperate(bool isOperate)
{
    this->isOperate = isOperate;
}

bool Status::getOperate()
{
    return this->isOperate;
}

void Status::setOverrideMode(int mode)
{
    this->overrideMode = mode;
}

int Status::getOverrideMode()
{
    return this->overrideMode;
}

void Status::setStatusChanged(bool isStatusChanged)
{
    this->isStatusChanged = isStatusChanged;
}

bool Status::getStatusChanged()
{
    return this->isStatusChanged;
}

Status::Status(/* args */)
{
    this->mode = 0;
    this->isBuzzerOn = false;
}

void Status::setMode(int mode)
{
    this->lastMode = this->mode;
    this->mode = mode;
}

int Status::getMode()
{
    return this->mode;
}

void Status::setBuzzer(bool isBuzzerOn)
{
    this->isBuzzerOn = isBuzzerOn;
}

bool Status::getBuzzer()
{
    return this->isBuzzerOn;
}

Status::~Status()
{
}
