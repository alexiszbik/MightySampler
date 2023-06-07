#ifndef SAMPLERPARAMETERS_H
#define SAMPLERPARAMETERS_H

#include <vector>
using namespace std;

enum ESamplerParameter {
    Volume = 0,

    Speed,
    FXSend,


    Count
};

struct SamplerParameter {
    const char* name;
    float value;
};

class SamplerParameterList : public vector<SamplerParameter> {
 public:
    SamplerParameterList() : vector<SamplerParameter>() {
        resize(Count);
        
        at(Volume) = {"Volume", 1};
        at(Speed) = {"Speed", 1};
        at(FXSend) = {"FXSend", 1};
    }
};

#endif