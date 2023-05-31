
#ifndef PATCH_H
#define PATCH_H

#include <string>
#include "fatfs.h"
#include "DisplayManager.h"

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

    ButtonDesc buttonDesc[SPLR_COUNT];

    void loadFile(const char* name,  FIL& SDFile);

    void loadPlayMode(const char* value, ButtonDesc* desc);

private:
    bool read(FIL& SDFile);

private:
    DisplayManager *display = DisplayManager::GetInstance();

    static const short bufferLen = 60;
    char buffer[bufferLen] = "";

    int currentButtonIndex = 0;

};

#endif //PATCH_H