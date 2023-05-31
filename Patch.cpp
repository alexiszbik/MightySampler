#include "Patch.h"
#include "StringTools.h"
#include "VectorTools.h"

static std::vector<const char*> buttons_tags = {"BUTTON_A", "BUTTON_B", "BUTTON_C", "BUTTON_D", "BUTTON_E", "BUTTON_F"};

#define SAMPLES_KEY "SAMPLES"
#define PLAYMODE_KEY "PLAYMODE"

#define PLAYMODE_KEY_Trigger "Trigger"
#define PLAYMODE_KEY_Gate "Gate"
#define PLAYMODE_KEY_OneShot "OneShot"

Patch::Patch() {
}

void Patch::loadFile(const char* name, FIL& SDFile) {
    f_open(&SDFile, name, (FA_OPEN_EXISTING | FA_READ));
    f_gets((char*)buffer, bufferLen, &SDFile); //first line is shit

    bool doIt = true;
    while (doIt) {
        doIt = read(SDFile);
    }

    f_close(&SDFile);
}

bool Patch::read(FIL& SDFile) {
    TCHAR* result = f_gets((char*)buffer, bufferLen, &SDFile);

    if (result == nullptr) {
        return false;
    }

    std::vector<std::string> splitted;
    tokenize(buffer, splitted);

    std::string main = splitted.at(0);
    
    auto cMain = main.c_str();
    
    int buttonIndex = -1;
    int iterator = 0;

    if (splitted.size() == 1) {
        //Why is there an extra caracter here?
        main.pop_back();

        for (auto tag : buttons_tags) {

            if (strEquals(cMain, tag)) {
                buttonIndex = iterator;
            }

            iterator++;
        }
    }

    if (buttonIndex >= 0) {
        currentButtonIndex = buttonIndex;

    } else if (splitted.size() > 1) {

        ButtonDesc* desc = &buttonDesc[currentButtonIndex];

        std::string arg = splitted.at(1);
        arg.erase(std::remove(arg.begin(), arg.end(), '\n'), arg.cend());
        arg.erase(std::remove(arg.begin(), arg.end(), ' '), arg.cend());

        if (strEquals(cMain, SAMPLES_KEY)) {
            strcpy(desc->sampleName, arg.c_str());
        }
        if (strEquals(cMain, PLAYMODE_KEY)) {
            loadPlayMode(arg.c_str(), desc);
        }
    }
    return true;
}

void Patch::loadPlayMode(const char* value, ButtonDesc* desc) {
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
