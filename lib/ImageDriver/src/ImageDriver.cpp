#include "ImageDriver.h"
#include "esp_log.h"
#include <cstdlib>
#include <algorithm>

#define TAG "IMAGEDRIVER"

ImageDriver::ImageDriver()
{
    imgSize = act_width * height;
    img = (char *)malloc(sizeof(char) * imgSize);
    for (int x = 0; x < imgSize; x++)
    {
        img[x] = 0;
    }
}

void ImageDriver::addGraph(int x1, int y1, int x2, int y2)
{
}

void ImageDriver::addImage()
{
}

void ImageDriver::addLine(int x1, int y1, int x2, int y2)
{
    // Bresenham algorithm (gradient <= 1)
    int dx = x2 - x1;
    int dy = y2 - y1;
    int D = (dy + dy) - dx;
    int y = y1;

    for (int x = x1; x <= x2; x++)
    {
        addPoint(x, y);
        if (D > 0)
        {
            y += 1;
            D = D - (dx + dx);
        }
        D += dy + dy;
    }
}

char pow(int base, int exp)
{
    if (exp == 0)
    {
        return 1;
    }
    char x = base;
    while (exp > 1)
    {
        x *= base;
        exp--;
    }
    return x;
}

/**
 *  draws a Vertical Line inside a Byte
 *  @param pos Index in Image Array
 *  @param leftOffset Empty 0s on the left (0-7)
 *  @param leftOffset Empty 0s on the right (0-7)
 *  @param _height Height of the Line
 */
void ImageDriver::drawVerticalLine(int pos, int leftOffset, int rightOffset, int _height)
{
    if (pos >= imgSize)
    {
        return;
    }
    char byte = (pow(2, 8 - leftOffset - rightOffset) - 1) << rightOffset;
    for (int z = 0; z < _height && pos < imgSize; z++)
    {
        img[pos] |= byte;
        pos += act_width; // Next row
    }
}

/**
 *  draws a square
 *  @param x X Position in Image
 *  @param y Y Position in Image
 *  @param _width Width of the square
 *  @param _height Height of the square
 */
void ImageDriver::addFilledRect(int x, int y, int _width, int _height)
{
    if (x >= width || y >= height)
    {
        return;
    }
    _width  = std::min(act_width - x, _width);
    _height = std::min(height - y, _height);

    int pos = cords2index(x,y);
    int leftOffset = x%8;
    int rightOffset = std::max(8 - (leftOffset + _width), 0);
    drawVerticalLine(pos, leftOffset, rightOffset, _height);
    _width -= 8 - leftOffset;

    while(_width > 0){
        pos += 1;
        leftOffset = 0;
        rightOffset = std::max(8 - (leftOffset + _width), 0);
        drawVerticalLine(pos, leftOffset, rightOffset, _height);
        _width -= 8;
    }
}

void ImageDriver::addPoint(int x, int y)
{
    int cord = cords2index(x, y);
    img[cord] = img[cord] | (1 << (7 - (x % 8)));
}

void ImageDriver::addText()
{
}

int ImageDriver::cords2index(int x, int y)
{
    return y * act_width + x / 8;
}

/*void ImageDriver::debug(){
    for(int x = 0; x < act_width*height; x++){
        ESP_LOGI(TAG, "ABC");
    }
}*/