#ifndef IMAGEDRIVER

#define IMAGEDRIVER

#define WIDTH 800
#define HEIGHT 480
#include <sys/_stdint.h>

class ImageDriver {
public:
    ImageDriver();
    ~ImageDriver();
    ImageDriver(const ImageDriver&) = delete;
    int width = WIDTH;
    int act_width = WIDTH / 8;
    int height = HEIGHT;
    int imgSize;
    uint8_t* img;
    void addText();
    void addImage();
    void addLine(int x1, int y1, int x2, int y2) const;
    void addPoint(int x, int y) const;
    void drawVerticalLine(int pos, int leftOffset, int rightOffset,
                          int height) const;
    void addFilledRect(int x, int y, int width, int height) const;
    void addGraph(int x1, int y1, int x2, int y2);
    //void debug();

private:
    int cords2index(int x, int y) const;
};

#endif
