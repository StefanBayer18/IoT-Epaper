class DisplayDriver{
    public: 
    DisplayDriver(int w, int h, int rpin, int dc, int cs, int busy);
    void sleep();
    void wait_idle();
    void sendCommand(int cmd);
    void reset();
    void sendData(int data);

    private:
    int width;
    int height;
    int reset_pin;
    int dc_pin;
    int cs_pin;
    int busy_pin;
};