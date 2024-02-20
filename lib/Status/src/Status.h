#include <dev_config.h>

class Status
{
private:
    int mode;
    int lastMode;
    int overrideMode;
    bool isStatusChanged;
    bool isBuzzerOn;
    bool isOperate;
    unsigned long lastTime;
    unsigned long currentTime;
public:
    Status(/* args */);
    void setMode(int mode);
    int getMode();

    static String modeToString(int mode);

    void setLastTime(unsigned long lastTime);
    unsigned long getLastTime();

    void setCurrentTime(unsigned long currentTime);
    unsigned long getCurrentTime();

    void setOperate(bool isOperate);
    bool getOperate();

    void setStatusChanged(bool isStatusChanged);
    bool getStatusChanged();

    void setOverrideMode(int mode);
    int getOverrideMode();

    void setLastMode(int lastMode);
    int getLastMode();

    void setBuzzer(bool isBuzzerOn);
    bool getBuzzer();

    ~Status();
};
