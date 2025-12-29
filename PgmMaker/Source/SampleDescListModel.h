#pragma once

#include <JuceHeader.h>
#include "../../PatchReader.h"

//==============================================================================
/*
    Component for displaying a single SampleDesc row with play button
*/
class SampleDescRowComponent : public juce::Component,
                                public juce::Button::Listener
{
public:
    //==============================================================================
    SampleDescRowComponent();
    ~SampleDescRowComponent() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    
    void setSampleDesc(const SampleDesc* sampleDesc, int rowNumber, bool isSelected);
    
private:
    //==============================================================================
    const SampleDesc* sampleDesc = nullptr;
    int rowNumber = -1;
    bool isSelected = false;
    juce::TextButton playButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDescRowComponent)
};

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
    juce::Component* refreshComponentForRow(int rowNumber, bool isRowSelected,
                                           juce::Component* existingComponentToUpdate) override;
    int getRowHeight(int rowNumber);
    
    void setSampleDescs(std::vector<SampleDesc>* sampleDescs);
    
private:
    //==============================================================================
    std::vector<SampleDesc>* sampleDescs;
    juce::OwnedArray<SampleDescRowComponent> components;
};

