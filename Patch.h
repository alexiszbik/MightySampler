
#ifndef PATCH_H
#define PATCH_H

#include <string>
#include "fatfs.h"
#include "DisplayManager.h"

#define SPLR_COUNT 6

std::vector buttons_tags = {"BUTTON_A", "BUTTON_B", "BUTTON_C", "BUTTON_D", "BUTTON_E", "BUTTON_F"};

enum EPlayMode {
    Trigger,
    Gate,
    OneShot
};

struct SamplerDesc {
    std::string sampleName;
    EPlayMode playMode;
};

class Patch
{
public:
    SamplerDesc samplerDesc[SPLR_COUNT];

    void loadFile(const char* name);

private:
    bool read();

private:
    DisplayManager *display = DisplayManager::GetInstance();

    static const short bufferLen = 60;
    char buffer[bufferLen] = "";

};

#endif //PATCH_H