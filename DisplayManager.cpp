
#include "DisplayManager.h"

DisplayManager* DisplayManager::singleton = nullptr;

DisplayManager *DisplayManager::GetInstance()
{
    if(singleton == nullptr){
        singleton = new DisplayManager();
    }
    return singleton;
}

void DisplayManager::Init(DaisySeed *hw) {
    /** Configure the Display */
    Display_t::Config disp_cfg;
    disp_cfg.driver_config.transport_config.Defaults();
    disp_cfg.driver_config.transport_config.i2c_config.pin_config.scl = hw->GetPin(11);
    disp_cfg.driver_config.transport_config.i2c_config.pin_config.sda = hw->GetPin(12);
    
    /** And Initialize */
    display.Init(disp_cfg);
}

void DisplayManager::Write(const char* title, const char* message) {
    display.Fill(false);
    display.SetCursor(0, 0);
    display.WriteString(title, Font_11x18, true);
    if (message) {
        display.SetCursor(0, fontHeight);
        display.WriteString(message, Font_11x18, true);
    }
    display.Update();
}