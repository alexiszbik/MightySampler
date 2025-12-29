#pragma once

#include <JuceHeader.h>
#include "../AudioFile/AudioFile.h"

//==============================================================================
/*
    AudioSource for playing a single sample from AudioFile
*/
class SamplePlayer : public juce::AudioSource
{
public:
    //==============================================================================
    SamplePlayer();
    ~SamplePlayer() override;
    
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    
    //==============================================================================
    /** Set the sample to play */
    void setSample(AudioFile<float>* audioFile);
    
    /** Start playing the sample */
    void start();
    
    /** Stop playing the sample */
    void stop();
    
    /** Check if currently playing */
    bool isPlaying() const;
    
    /** Set callback for when playback stops */
    void setPlaybackStoppedCallback(std::function<void()> callback);
    
private:
    //==============================================================================
    AudioFile<float>* currentSample = nullptr;
    int currentPosition = 0;
    bool playing = false;
    double currentSampleRate = 44100.0;
    std::function<void()> playbackStoppedCallback;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplePlayer)
};

