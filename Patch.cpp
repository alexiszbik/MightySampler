#include "Patch.h"
#include "Tools/StringTools.h"
#include "Tools/VectorTools.h"

static constexpr const char SAMPLE_KEY[]     = "SAMPLE";
static constexpr const char LAYER_KEY[]      = "LAYER";
static constexpr const char PLAYMODE_KEY[]   = "PLAYMODE";
static constexpr const char SAMPLE_ID_KEY[]  = "SAMPLE_ID";
static constexpr const char PITCH_KEY[]      = "PITCH";
static constexpr const char REVERSE_KEY[]    = "REVERSE";

static constexpr const char PLAYMODE_KEY_Trigger[] = "Trigger";
static constexpr const char PLAYMODE_KEY_Gate[]    = "Gate";
static constexpr const char PLAYMODE_KEY_OneShot[] = "OneShot";


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
            layers.push_back(LayerData());
            currentLayerData = &layers[layers.size() - 1];
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
            sampleDescs.push_back(SampleDesc());
            currentSampleDesc = &sampleDescs[sampleDescs.size() - 1];
            strcpy(currentSampleDesc->sampleName, arg.c_str());
        }

        if (strEquals(cMain, SAMPLE_ID_KEY)) {
            int splId = str_to_int(arg.c_str());
            currentLayerData->sampleId = splId;
        }
        
        if (strEquals(cMain, PLAYMODE_KEY) && currentLayerData) {
            loadPlayMode(arg.c_str(), currentLayerData);
        }

        if (strEquals(cMain, PITCH_KEY) && currentLayerData) {
            int pitch = str_to_int(arg.c_str());
            currentLayerData->pitch = pitch;
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

