#include "SamplePlayer.h"

//==============================================================================
SamplePlayer::SamplePlayer()
{
}

SamplePlayer::~SamplePlayer()
{
    releaseResources();
}

void SamplePlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    currentPosition = 0;
}

void SamplePlayer::releaseResources()
{
    currentSample = nullptr;
    currentPosition = 0;
}

void SamplePlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!playing || currentSample == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    int numChannels = currentSample->getNumChannels();
    int numSamples = currentSample->getNumSamplesPerChannel();
    
    if (numChannels == 0 || numSamples == 0)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* rightChannel = bufferToFill.buffer->getNumChannels() > 1 
                        ? bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample)
                        : nullptr;
    
    int samplesToCopy = juce::jmin(bufferToFill.numSamples, numSamples - currentPosition);
    
    for (int i = 0; i < samplesToCopy; ++i)
    {
        if (currentPosition < numSamples)
        {
            float sampleValue = currentSample->samples[0][currentPosition];
            leftChannel[i] = sampleValue;
            
            if (rightChannel != nullptr)
            {
                if (numChannels > 1)
                    rightChannel[i] = currentSample->samples[1][currentPosition];
                else
                    rightChannel[i] = sampleValue; // Mono to stereo
            }
            
            currentPosition++;
        }
    }
    
    // Clear remaining buffer if sample ended
    if (samplesToCopy < bufferToFill.numSamples)
    {
        bufferToFill.buffer->clear(samplesToCopy, bufferToFill.numSamples - samplesToCopy);
        playing = false;
        currentPosition = 0;
        
        // Notify that playback stopped
        if (playbackStoppedCallback)
        {
            playbackStoppedCallback();
        }
    }
}

void SamplePlayer::setSample(AudioFile<float>* audioFile)
{
    currentSample = audioFile;
    currentPosition = 0;
}

void SamplePlayer::start()
{
    if (currentSample != nullptr && currentSample->getNumSamplesPerChannel() > 0)
    {
        currentPosition = 0;
        playing = true;
    }
}

void SamplePlayer::stop()
{
    playing = false;
    currentPosition = 0;
}

bool SamplePlayer::isPlaying() const
{
    return playing;
}

void SamplePlayer::setPlaybackStoppedCallback(std::function<void()> callback)
{
    playbackStoppedCallback = callback;
}

