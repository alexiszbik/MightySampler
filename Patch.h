
#ifndef PATCH_H
#define PATCH_H

#include <string>
#include "fatfs.h"
#include "DisplayManager.h"

#define SPLR_COUNT 6

#define SAMPLE_FILE_NAME_MAX 32

#define SAMPLES_KEY "SAMPLES"
#define PLAYMODE_KEY "PLAYMODE"

enum EPlayMode {
    Trigger,
    Gate,
    OneShot
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

    void loadFile(const char* name);

private:
    bool read();

private:
    DisplayManager *display = DisplayManager::GetInstance();

    static const short bufferLen = 60;
    char buffer[bufferLen] = "";

    int currentButtonIndex = 0;

};

#endif //PATCH_H