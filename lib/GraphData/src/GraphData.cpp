#include <GraphData.h>

GraphData::GraphData(int _size){
    size = _size;
    bitSize = sizeof(uint8_t) * size;
    start = 0;
    end = 0;

    data = (int8_t *)malloc(bitSize);
    for(int x = 0; x < size; x++){
        data[x] = x;
    }

    saveData();
}

void GraphData::addVal(int val){
    getData();
    data[end] = val;
    end = (end + 1) % size;
    if(end == start){
        start = (start + 1) % size;
    }
    saveData();
}

void GraphData::saveData(){
    esp_err_t err;
    nvs_handle_t nvsHandle = openNVS(NVS_READWRITE);

    err = nvs_set_u8(nvsHandle, STARTKEY, start);
    if (err != ESP_OK)
        printf("Error writing start\n");

    err = nvs_set_u8(nvsHandle, ENDKEY, end);
    if (err != ESP_OK) 
        printf("Error writing end\n");

    err = nvs_set_blob(nvsHandle, DATAKEY, data, bitSize);
    if (err != ESP_OK) 
        printf("Error writing data\n");

    nvs_close(nvsHandle);
}

void GraphData::getData(){
    esp_err_t err;
    nvs_handle_t nvsHandle = openNVS(NVS_READONLY);
    size_t readSize;

    err = nvs_get_u8(nvsHandle, STARTKEY, &start);
    if(err != ESP_OK){
        printf("Error reading start\n");
    }

    err = nvs_get_u8(nvsHandle, ENDKEY, &end);
    if(err != ESP_OK){
        printf("Error reading end\n");
    }

    err = nvs_get_blob(nvsHandle, DATAKEY, data, &readSize);
    if(err != ESP_OK){
        printf("Error reading data\n");
    }
    else if (readSize != bitSize)
    {
        printf("Error reading data\nRead invalid data\n");
    }
    
    nvs_close(nvsHandle);
}

nvs_handle_t GraphData::openNVS(nvs_open_mode_t mode){
    esp_err_t err;

    err = nvs_flash_init();
    if (err != ESP_OK) {
        printf("NVS init failed\n");
        return;
    }

    nvs_handle_t nvsHandle;

    err = nvs_open(STORAGE, mode, &nvsHandle);
    if (err != ESP_OK) {
        printf("Error opening NVS\n");
        return;
    }

    return nvsHandle;
}