#include "Patch.h"
#include "StringTools.h"

void Patch::loadFile(const char* name) {
    f_open(&SDFile, name, (FA_OPEN_EXISTING | FA_READ));
    f_gets((char*)buffer, bufferLen, &SDFile); //first line is shit

    bool doIt = true;
    while (doIt) {
        doIt = read();
    }

    f_close(&SDFile);
}

bool Patch::read() {
    TCHAR* result = f_gets((char*)buffer, bufferLen, &SDFile);

    if (result == nullptr) {
        return false;
    }

    std::vector<std::string> splitted;
    tokenize(buffer, splitted);

    display->Write(splitted);
    return true;
}

