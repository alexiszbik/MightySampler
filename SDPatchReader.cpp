#include "SDPatchReader.h"

SDPatchReader::SDPatchReader(Patch* patch) {
    this->patch = patch;
    pr = new PatchReader(this->patch);
}

SDPatchReader::~SDPatchReader() {
    delete pr;
}

void SDPatchReader::loadFile(const char* name, FIL& SDFile) {
    f_open(&SDFile, name, (FA_OPEN_EXISTING | FA_READ));
    f_gets((char*)buffer, bufferLen, &SDFile); //first line is shit? do we really need a first blank line?

    bool doIt = true;
    while (doIt) {
        doIt = read(SDFile);
    }

    f_close(&SDFile);
}

bool SDPatchReader::read(FIL& SDFile) {
    TCHAR* result = f_gets((char*)buffer, bufferLen, &SDFile);

    if (result == nullptr) {
        return false; //stop reading
    }

    return pr->read(buffer);

}

