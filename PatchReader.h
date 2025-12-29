#pragma once

#include <string>
#include <vector>

#include "Patch.h"
//#include "DaisyYMNK/DaisyYMNK.h"

using namespace std;

class PatchReader
{
public:
    PatchReader(Patch* patch);

    bool read(const char* buffer);

    void loadPlayMode(const char* value, LayerData* ld);

private:
    Patch* patch;
    //DisplayManager *display = DisplayManager::GetInstance();

    SampleDesc* currentSampleDesc = nullptr;
    LayerData* currentLayerData = nullptr;

};
