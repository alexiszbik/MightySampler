#include "Patch.h"
#include "Tools/StringTools.h"
#include "Tools/VectorTools.h"

#define SAMPLE_KEY "SAMPLE"
#define LAYER_KEY "LAYER"
#define PLAYMODE_KEY "PLAYMODE"

#define REVERSE_KEY "REVERSE"

#define PLAYMODE_KEY_Trigger "Trigger"
#define PLAYMODE_KEY_Gate "Gate"
#define PLAYMODE_KEY_OneShot "OneShot"

Patch::Patch() {
}

void Patch::loadFile(const char* name, FIL& SDFile) {
    f_open(&SDFile, name, (FA_OPEN_EXISTING | FA_READ));
    f_gets((char*)buffer, bufferLen, &SDFile); //first line is shit? do we really need a first blank line?

    bool doIt = true;
    while (doIt) {
        doIt = read(SDFile);
    }

    f_close(&SDFile);
}

bool Patch::read(FIL& SDFile) {
    TCHAR* result = f_gets((char*)buffer, bufferLen, &SDFile);

    if (result == nullptr) {
        return false; //stop reading
    }

    std::vector<std::string> splitted;
    tokenize(buffer, splitted); //split line with " "

    std::string main = splitted.at(0);
    
    auto cMain = main.c_str();

    display->Write(splitted, true);
    
    if (splitted.size() == 1) {
        //Why is there an extra caracter here?
        main.pop_back();

        if (strEquals(cMain, LAYER_KEY)) {

            currentLayerIndex++;
            layers.push_back(LayerData());

            currentLayerData = &layers[currentLayerIndex];
        }
        if (strEquals(cMain, REVERSE_KEY)) {
            currentLayerData->isReverse = true;
        }
    }

    if (splitted.size() > 1) {

        std::string arg = splitted.at(1);
        arg.erase(std::remove(arg.begin(), arg.end(), '\n'), arg.cend());
        arg.erase(std::remove(arg.begin(), arg.end(), ' '), arg.cend());

        if (strEquals(cMain, SAMPLE_KEY)) {

            currentSampleIndex++;
            sampleDescs.push_back(SampleDesc());

            currentSampleDesc = &sampleDescs[currentSampleIndex];
            strcpy(currentSampleDesc->sampleName, arg.c_str());
            currentLayerData->sampleId = currentSampleIndex;
        }
        
        if (strEquals(cMain, PLAYMODE_KEY) && currentLayerData) {
            loadPlayMode(arg.c_str(), currentLayerData);
        }
    }
    return true;
}

void Patch::loadPlayMode(const char* value, LayerData* ld) {
    if (strEquals(value, PLAYMODE_KEY_Trigger)) {
        ld->playMode = Trigger;
    } 
    else if (strEquals(value, PLAYMODE_KEY_Gate)) {
        ld->playMode = Gate;
    }  
    else if (strEquals(value, PLAYMODE_KEY_OneShot)) {
        ld->playMode = OneShot;
    } else {
        
    }
}
