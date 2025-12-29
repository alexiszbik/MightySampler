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

