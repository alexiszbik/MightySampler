#include "Patch.h"
#include "Tools/StringTools.h"
#include "Tools/VectorTools.h"

#define SAMPLE_KEY "SAMPLE"
#define PLAYMODE_KEY "PLAYMODE"

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
    tokenize(buffer, splitted);

    std::string main = splitted.at(0);
    
    auto cMain = main.c_str();
    /*
    if (splitted.size() == 1) {
        //Why is there an extra caracter here?
        main.pop_back();
    }
    */

    if (splitted.size() > 1) {

        std::string arg = splitted.at(1);
        arg.erase(std::remove(arg.begin(), arg.end(), '\n'), arg.cend());
        arg.erase(std::remove(arg.begin(), arg.end(), ' '), arg.cend());

        if (strEquals(cMain, SAMPLE_KEY)) {

            currentButtonIndex++;
            sampleDescs.push_back(SampleDesc());

            desc = &sampleDescs[currentButtonIndex];
            strcpy(desc->sampleName, arg.c_str());
        }
        if (strEquals(cMain, PLAYMODE_KEY) && desc) {
            loadPlayMode(arg.c_str(), desc);
        }
    }
    return true;
}

void Patch::loadPlayMode(const char* value, SampleDesc* desc) {
    if (strEquals(value, PLAYMODE_KEY_Trigger)) {
        desc->playMode = Trigger;
    } 
    else if (strEquals(value, PLAYMODE_KEY_Gate)) {
        desc->playMode = Gate;
    }  
    else if (strEquals(value, PLAYMODE_KEY_OneShot)) {
        desc->playMode = OneShot;
    } else {
        
    }
}
