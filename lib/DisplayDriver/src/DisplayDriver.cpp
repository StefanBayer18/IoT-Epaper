#include "DisplayDriver.h"

DisplayDriver::DisplayDriver(ImageDriver _img, int rpin, int dc, int cs, int busy){
    img = _img;
    reset_pin = rpin;
    dc_pin = dc;
    cs_pin = cs;
    busy_pin = busy;
}

void DisplayDriver::show(){
    
}

void DisplayDriver::sendData(char data){
    
}

void DisplayDriver::sendCommand(char cmd){

}

void DisplayDriver::reset(){

}

void DisplayDriver::wait_idle(){

}

void DisplayDriver::sleep(){

}


