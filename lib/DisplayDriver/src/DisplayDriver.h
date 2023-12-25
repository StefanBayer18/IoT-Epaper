#include <ImageDriver.h>

class DisplayDriver{
    public: 
    DisplayDriver(int rpin, int dc, int cs, int busy);
    void show(ImageDriver _img);
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