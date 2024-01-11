#include "dev_config.h"

class outputHandler
{
private:
    int pin;
    bool isOn;

public:

    outputHandler(int pin);
    outputHandler();

    void on();
    void off();
    void toggle();


};


