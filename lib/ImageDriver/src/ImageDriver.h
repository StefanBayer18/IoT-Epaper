#ifndef IMAGEDRIVER

#define IMAGEDRIVER

#define WIDTH 800
#define HEIGHT 480

class ImageDriver{
    public:
    ImageDriver();
    int width = WIDTH;
    int act_width = WIDTH/8;
    int height = HEIGHT;
    int imgSize;
    char* img;
    void addText();
    void addImage();
    void addLine(int x1, int y1, int x2, int y2);
    void addPoint(int x, int y);
    void drawVerticalLine(int x, int y, int width, int height);
    void addFilledRect(int x, int y, int width, int height);
    void addGraph(int x1, int y1, int x2, int y2);
    //void debug();
    
    private:
    int cords2index(int x, int y); 
};

#endif