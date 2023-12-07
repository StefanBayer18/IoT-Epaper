#include "DisplayDriver.h"

DisplayDriver::DisplayDriver(int w, int h, int rpin, int dc, int cs, int busy){
    width = w;
    height = h;
    reset_pin = rpin;
    dc_pin = dc;
    cs_pin = cs;
    busy_pin = busy;
}

void DisplayDriver::sendData(int data){
    
}

void DisplayDriver::sendCommand(int cmd){

}

void DisplayDriver::reset(){

}

void DisplayDriver::wait_idle(){

}

void DisplayDriver::sleep(){

}


