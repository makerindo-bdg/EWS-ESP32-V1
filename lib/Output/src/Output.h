#include <dev_config.h>

class Output
{
private:
    int pin;
    bool isOn;

public:

    Output(int pin);
    Output();

    void on();
    void off();
    void toggle();


};


