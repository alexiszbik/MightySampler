#pragma once

#include <JuceHeader.h>
#include "LayerDataListModel.h"

//==============================================================================
/*
    ListBox component wrapper for LayerData display
*/
class LayerDataListBox : public juce::Component
{
public:
    //==============================================================================
    LayerDataListBox();
    ~LayerDataListBox() override = default;
    
    void setLayers(std::vector<LayerData>* layers);
    void updateContent();
    void resized() override;
    
private:
    //==============================================================================
    juce::ListBox listBox;
    LayerDataListModel model;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayerDataListBox)
};

