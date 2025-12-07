
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
    EPlayMode playMode = OneShot;
};

class Patch
{
public:
    Patch();

    void loadFile(const char* name,  FIL& SDFile);

private:
    bool read(FIL& SDFile);

    void loadPlayMode(const char* value, SampleDesc* desc);

public:
    vector<SampleDesc> sampleDescs;

private:
    DisplayManager *display = DisplayManager::GetInstance();

    //Utils for reading patch
    static const short bufferLen = 60; //Maybe 60 is a lot, but it's working
    char buffer[bufferLen] = "";

    int currentButtonIndex = -1;
    SampleDesc* desc = nullptr;

};

#endif //PATCH_H