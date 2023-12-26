#include "ImageDriver.h"
#include "esp_log.h"
#define TAG "IMAGEDRIVER"

ImageDriver::ImageDriver(){

}

void ImageDriver::addGraph(int x1, int y1, int x2, int y2){

}

void ImageDriver::addImage(){

}

void ImageDriver::addLine(int x1, int y1, int x2, int y2){
    
}

void ImageDriver::addPoint(int x, int y){
    int cord = cords2index(x, y);
    img[cord] = img[cord] | (1 << (7 - (x%8)));
}

void ImageDriver::addText(){

}

int ImageDriver::cords2index(int x, int y){
    return y*act_width + x/8;
}

void ImageDriver::debug(){
    for(int x = 0; x < act_width*height; x++){
        ESP_LOGI(TAG, "ABC");
    }
}