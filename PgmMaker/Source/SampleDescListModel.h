#pragma once

#include <JuceHeader.h>
#include "../../PatchReader.h"

//==============================================================================
/*
    Component for the Play button in the SampleDesc table
*/
class PlayButtonComponent : public juce::Component,
                            public juce::Button::Listener
{
public:
    //==============================================================================
    PlayButtonComponent();
    ~PlayButtonComponent() override = default;
    
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    
    void setSampleDesc(const SampleDesc* sampleDesc);
    void setPlayCallback(std::function<bool(const juce::String&)> callback);
    void updateButtonState(bool isPlaying);
    juce::String getSampleName() const;
    
private:
    //==============================================================================
    const SampleDesc* sampleDesc = nullptr;
    juce::TextButton playButton;
    std::function<bool(const juce::String&)> playCallback;
    bool currentlyPlaying = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayButtonComponent)
};

//==============================================================================
/*
    Model class for displaying SampleDesc items in a TableListBox
*/
class SampleDescListModel : public juce::TableListBoxModel
{
public:
    //==============================================================================
    SampleDescListModel();
    
    //==============================================================================
    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                            juce::Component* existingComponentToUpdate) override;
    
    void setSampleDescs(std::vector<SampleDesc>* sampleDescs);
    void setPlayCallback(std::function<bool(const juce::String&)> callback);
    void updateButtonStates(const juce::String& currentlyPlayingSample);
    
    enum ColumnIds
    {
        Name = 1,
        Size = 2,
        Channels = 3,
        SampleRate = 4,
        Play = 5
    };
    
private:
    //==============================================================================
    std::vector<SampleDesc>* sampleDescs = nullptr;
    juce::OwnedArray<PlayButtonComponent> playButtonComponents;
    std::function<bool(const juce::String&)> playCallback;
};

