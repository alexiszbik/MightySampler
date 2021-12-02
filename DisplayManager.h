
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"

#include <string>
#include <vector>

using namespace daisy;

using Display_t = OledDisplay<SSD130xI2c128x64Driver>;

class DisplayManager
{
protected:
    DisplayManager() {};

    static DisplayManager* singleton;
public:
    static DisplayManager *GetInstance();

    void Init(DaisySeed *hw);
    void Write(const char* title, const char* message = nullptr);
    void Write(std::vector<std::string> messages);
    void Write(int value);
    
private:
    Display_t display;
    const int fontHeight = 18;

    char strbuff[24];

};


#endif //DISPLAY_MANAGER_H