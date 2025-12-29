#pragma once

#include <JuceHeader.h>
#include "../../PatchReader.h"

//==============================================================================
/*
    Model class for displaying LayerData items in a ListBox
*/
class LayerDataListModel : public juce::ListBoxModel
{
public:
    //==============================================================================
    LayerDataListModel(std::vector<LayerData>* layers);
    
    //==============================================================================
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, 
                          int width, int height, bool rowIsSelected) override;
    
    void setLayers(std::vector<LayerData>* layers);
    
private:
    //==============================================================================
    std::vector<LayerData>* layers;
    
    juce::String getPlayModeString(EPlayMode mode);
};

