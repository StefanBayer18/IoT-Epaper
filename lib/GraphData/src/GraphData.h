#ifndef GRAPHDATA
#define GRAPHDATA

#include "nvs_flash.h"

#include <optional>
#define STORAGE "GraphData"

#define DATAKEY "DataKey"
#define STARTKEY "StartKey"
#define ENDKEY "EndKey"

class GraphData {
public:
    GraphData(size_t _size);
    void addVal(int val);
    void saveData() const;
    void getData();

    size_t size;
    size_t bitSize;

    //Used for FiFo Storage
    uint8_t* data;
    uint8_t start;
    uint8_t end;

private:
    static std::optional<nvs_handle_t> openNVS(nvs_open_mode_t mode);
};

#endif
