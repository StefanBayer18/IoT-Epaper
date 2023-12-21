#include <DisplayDriver.h>

extern "C"
{
    void app_main()
    {
        DisplayDriver display = DisplayDriver(800,480,1,1,1,1);
        display.sendData('a');
        display.show();
    }
}