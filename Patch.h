
#ifndef PATCH_H
#define PATCH_H

#include <string>
#include <vector>

#include "fatfs.h"
#include "DaisyYMNK/DisplayManager.h"

#define SAMPLE_FILE_NAME_MAX 64

enum EPlayMode {
    Trigger = 0,
    Gate,
    OneShot
};

struct SampleDesc {
    char sampleName[SAMPLE_FILE_NAME_MAX];
};

struct LayerData
{
    EPlayMode playMode = OneShot;
};

class Patch
{
public:
    Patch();

    void loadFile(const char* name,  FIL& SDFile);

private:
    bool read(FIL& SDFile);

    void loadPlayMode(const char* value, LayerData* ld);

public:
    vector<SampleDesc> sampleDescs;
    vector<LayerData> layers;

private:
    DisplayManager *display = DisplayManager::GetInstance();

    //Utils for reading patch
    static const short bufferLen = 60; //Maybe 60 is a lot, but it's working
    char buffer[bufferLen] = "";

    short currentSampleIndex = -1;
    short currentLayerIndex = -1;
    SampleDesc* currentSampleDesc = nullptr;
    LayerData* currentLayerData = nullptr;

};

#endif //PATCH_H