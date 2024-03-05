#include "Output.h"

Output::Output(int pin)
{
    if (pin >= 0 && pin <= 40)
    {
        this->pin = pin;
        pinMode(pin, OUTPUT);
    }
}

Output::Output()
{
}

void Output::on()
{
    if (pin >= 0 && pin <= 40)
    {
        digitalWrite(pin, HIGH);
        isOn = true;
    }
}

void Output::off()
{
    if (pin >= 0 && pin <= 40)
    {
        digitalWrite(pin, LOW);
        isOn = false;
    }
}

void Output::toggle()
{
    if (pin >= 0 && pin <= 40)
    {
        if (isOn)
        {
            off();
        }
        else
        {
            on();
        }
    }
}