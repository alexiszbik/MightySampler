#pragma once

#include <JuceHeader.h>
#include "LayerDataListBox.h"
#include "SampleDescListBox.h"
#include "SampleManager.h"
#include "SamplePlayer.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    juce::TabbedComponent tabbedComponent;
    LayerDataListBox layerDataListBox;
    SampleDescListBox sampleDescListBox;
    PatchReader patchReader;
    SampleManager sampleManager;
    SamplePlayer samplePlayer;
    juce::String currentlyPlayingSample;
    
    bool onPlaySample(const juce::String& sampleName);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
