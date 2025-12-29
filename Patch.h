#pragma once

#include <vector>

using namespace std;

#define SAMPLE_FILE_NAME_MAX 64

enum EPlayMode {
    Trigger = 0,
    Gate,
    OneShot
};

struct SampleData
{
    size_t sampleSize = 0;
    int16_t* data;
    uint8_t sampleChanCount = 1;
    double sampleRate = 44100;
};

struct SampleDesc {
    char sampleName[SAMPLE_FILE_NAME_MAX];
    SampleData sampleData;
};

struct LayerData {
    uint8_t sampleId = 0;
    EPlayMode playMode = OneShot;
    bool isReverse = false;
    int pitch = 0;
};

struct Patch 
{
    vector<SampleDesc> sampleDescs;
    vector<LayerData> layers;
};
