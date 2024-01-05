#include "ImageDriver.h"
#include "esp_log.h"
#include <cstdlib>
#include <algorithm>

#define TAG "IMAGEDRIVER"

ImageDriver::ImageDriver(int _width, int _height){
        width = _width;
        height = _height;
        act_width = width/8;
        imgSize = act_width * height;
        img = new uint8_t[imgSize]();
}

ImageDriver::~ImageDriver() {
    delete[] img;
}

void ImageDriver::addGraph(int x1, int y1, int x2, int y2) {
    // TODO: Implement
}

void ImageDriver::addImage() {
    // TODO: Implement
}

void ImageDriver::addLine(int x1, int y1, int x2, int y2) const {
    // Bresenham algorithm (gradient <= 1)
    const int dx = x2 - x1;
    const int dy = y2 - y1;
    int D = (dy + dy) - dx;
    int y = y1;

    for (int x = x1; x <= x2; x++) {
        addPoint(x, y);
        if (D > 0) {
            y += 1;
            D = D - (dx + dx);
        }
        D += dy + dy;
    }
}

static int pow2(uint8_t exp) {
    return 1 << exp;
}

/**
 *  draws a Vertical Line inside a Byte
 *  @param pos Index in Image Array
 *  @param leftOffset Empty 0s on the left (0-7)
 *  @param rightOffset Empty 0s on the right (0-7)
 *  @param height Height of the Line
 */
void ImageDriver::drawVerticalLine(int pos, int leftOffset, int rightOffset,
                                   int height) const {
    if (pos >= imgSize) {
        return;
    }
    const uint8_t byte = (pow2(8u - leftOffset - rightOffset) - 1) <<
                         rightOffset;
    for (int z = 0; z < height && pos < imgSize; z++) {
        img[pos] |= byte;
        pos += act_width; // Next row
    }
}

/**
 *  draws a square
 *  @param x X Position in Image
 *  @param y Y Position in Image
 *  @param width Width of the square
 *  @param height Height of the square
 */
void ImageDriver::addFilledRect(int x, int y, int width, int height) const {
    if (x >= this->width || y >= this->height) {
        return;
    }
    width = std::min(act_width - x, width);
    height = std::min(this->height - y, height);

    int pos = cords2index(x, y);
    int leftOffset = x % 8;
    int rightOffset = std::max(8 - (leftOffset + width), 0);
    drawVerticalLine(pos, leftOffset, rightOffset, height);
    width -= 8 - leftOffset;

    while (width > 0) {
        pos += 1;
        leftOffset = 0;
        rightOffset = std::max(8 - (leftOffset + width), 0);
        drawVerticalLine(pos, leftOffset, rightOffset, height);
        width -= 8;
    }
}

void ImageDriver::addPoint(int x, int y) const {
    const int coord = cords2index(x, y);
    img[coord] = img[coord] | (1 << (7 - (x % 8)));
}

void ImageDriver::addText() {
    // TODO: Implement
}

int ImageDriver::cords2index(int x, int y) const {
    return y * act_width + x / 8;
}

/*void ImageDriver::debug(){
    for(int x = 0; x < act_width*height; x++){
        ESP_LOGI(TAG, "ABC");
    }
}*/
