#pragma once

#include <JuceHeader.h>
#include "SampleDescListModel.h"

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

