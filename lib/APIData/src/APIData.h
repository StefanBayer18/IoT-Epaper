#ifndef APIDATA
#define APIDATA

class APIData{
    public:
    APIData();
    void getData();
    float temperature();
    float humidity();

    private:
    float temp;
    float humi;
    int day;
    int minute;
    int hour;
};

#endif