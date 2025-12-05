
#ifndef PATCH_H
#define PATCH_H

#include <string>
#include "fatfs.h"
#include "DaisyYMNK/DisplayManager.h"

#define SPLR_COUNT 6
#define SAMPLE_FILE_NAME_MAX 64

enum EPlayMode {
    Trigger = 0,
    Gate = 1,
    OneShot = 2
};

struct ButtonDesc {
    char sampleName[SAMPLE_FILE_NAME_MAX];
    EPlayMode playMode;
};

class Patch
{
public:
    Patch();

    void loadFile(const char* name,  FIL& SDFile);

private:
    bool read(FIL& SDFile);

    void loadPlayMode(const char* value, ButtonDesc* desc);

public:
    ButtonDesc buttonDesc[SPLR_COUNT];

private:
    DisplayManager *display = DisplayManager::GetInstance();

    //Utils for reading patch
    static const short bufferLen = 60; //Maybe 60 is a lot, but it's working
    char buffer[bufferLen] = "";
    int currentButtonIndex = -1;
    ButtonDesc* desc = nullptr;

};

#endif //PATCH_H