#pragma once

#include <JuceHeader.h>
#include "../../PatchReader.h"

//==============================================================================
/*
    Model class for displaying SampleDesc items in a ListBox
*/
class SampleDescListModel : public juce::ListBoxModel
{
public:
    //==============================================================================
    SampleDescListModel(std::vector<SampleDesc>* sampleDescs);
    
    //==============================================================================
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, 
                          int width, int height, bool rowIsSelected) override;
    
    void setSampleDescs(std::vector<SampleDesc>* sampleDescs);
    
private:
    //==============================================================================
    std::vector<SampleDesc>* sampleDescs;
};

//==============================================================================
/*
    ListBox component wrapper for SampleDesc display
*/
class SampleDescListBox : public juce::Component
{
public:
    //==============================================================================
    SampleDescListBox();
    ~SampleDescListBox() override = default;
    
    void setSampleDescs(std::vector<SampleDesc>* sampleDescs);
    void updateContent();
    void resized() override;
    
private:
    //==============================================================================
    juce::ListBox listBox;
    SampleDescListModel model;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDescListBox)
};

