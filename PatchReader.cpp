
#include "PatchReader.h"
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

PatchReader::PatchReader(Patch* patch) {
    this->patch = patch;
}

char *remove_last_space_const(const char *str)
{
    size_t len = strlen(str);

    if (len > 0 && str[len - 1] == ' ')
        len--;

    char *out = (char*)malloc(len + 1);
    if (!out)
        return NULL;

    memcpy(out, str, len);
    out[len] = '\0';

    return out;
}

bool PatchReader::read(const char* buffer) {

    /*
    display->WriteNow(buffer);
    System::Delay(500);
    */

    std::vector<std::string> splitted;
    tokenize(buffer, splitted); //split line with " "
    
    if (splitted.size() == 0) {
        return true;
    }

    std::string main = splitted.at(0);
    
    auto cMain = main.c_str();
    
    if (splitted.size() == 1) {
        //Why is there an extra caracter here?
        //main.pop_back();

        // Supprime \r et \n
        main.erase(std::remove(main.begin(), main.end(), '\r'), main.end());
        main.erase(std::remove(main.begin(), main.end(), '\n'), main.end());

        cMain = main.c_str();

        //main = remove_last_space_const(cMain);

        if (strEquals(cMain, LAYER_KEY)) {
            patch->layers.push_back(LayerData());
            currentLayerData = &patch->layers[patch->layers.size() - 1];
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
            patch->sampleDescs.push_back(SampleDesc());
            currentSampleDesc = &patch->sampleDescs[patch->sampleDescs.size() - 1];
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

void PatchReader::loadPlayMode(const char* value, LayerData* ld) {
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

