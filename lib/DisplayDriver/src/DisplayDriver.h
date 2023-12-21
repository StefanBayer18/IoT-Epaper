#include <ImageDriver.h>

class DisplayDriver{
    public: 
    DisplayDriver(ImageDriver _img, int rpin, int dc, int cs, int busy);
    void show();
    void sleep();
    void wait_idle();
    void sendCommand(char cmd);
    void reset();
    void sendData(char data);

    private:
    ImageDriver img;
    int width;
    int height;
    int reset_pin;
    int dc_pin;
    int cs_pin;
    int busy_pin;
};