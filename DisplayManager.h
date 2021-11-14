
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"

using namespace daisy;

using Display_t = OledDisplay<SSD130xI2c128x64Driver>;

class DisplayManager
{
public:
    void Init(DaisySeed *hw);
    void Write(const char* message);
    
private:
    Display_t display;

};


#endif //DISPLAY_MANAGER_H