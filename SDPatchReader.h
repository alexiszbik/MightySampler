#pragma once

#include <vector>

#include "fatfs.h"

#include "PatchReader.h"

#include "DaisyYMNK/DaisyYMNK.h"

class SDPatchReader
{
public:
    SDPatchReader(Patch* patch);
    ~SDPatchReader();

    void loadFile(const char* name, FIL& SDFile);

private:
    bool read(FIL& SDFile);

public:
    

private:
    PatchReader* pr;
    Patch* patch;

    //Utils for reading patch
    static const short bufferLen = 60; //Maybe 60 is a lot, but it's working
    char buffer[bufferLen] = "";

};