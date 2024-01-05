#include "nvs_flash.h"

#ifndef GRAPHDATA

#define GRAPHDATA

#define STORAGE "GraphData"

#define DATAKEY "DataKey"
#define STARTKEY "StartKey"
#define ENDKEY "EndKey"

class GraphData{
    public:
    GraphData(int _size);
    void addVal(int val);
    void saveData();
    void getData();
    
    int size;
    size_t bitSize;

    //Used for FiFo Storage
    int8_t* data;
    uint8_t start;
    uint8_t end;

    private:
    nvs_handle_t openNVS(nvs_open_mode_t mode);
};

#endif