#pragma once

#include <JuceHeader.h>
#include "../AudioFile/AudioFile.h"
#include "../../PatchReader.h"
#include <memory>
#include <map>

//==============================================================================
/*
    Class to manage loading and storing audio samples in memory
*/
class SampleManager
{
public:
    //==============================================================================
    SampleManager();
    ~SampleManager();
    
    //==============================================================================
    /** Load all samples from the sampler directory */
    bool loadAllSamples(const juce::String& samplerDirectory = "~/Documents/sampler");
    
    /** Load a specific sample by name */
    bool loadSample(const juce::String& sampleName, const juce::String& samplerDirectory = "~/Documents/sampler");
    
    /** Get loaded audio data for a sample by name */
    AudioFile<float>* getSample(const juce::String& sampleName);
    
    /** Get loaded audio data for a sample by index */
    AudioFile<float>* getSample(size_t index);
    
    /** Check if a sample is loaded */
    bool isSampleLoaded(const juce::String& sampleName);
    
    /** Get the number of loaded samples */
    size_t getNumLoadedSamples() const;
    
    /** Clear all loaded samples */
    void clear();
    
    /** Update sample descriptions with loaded audio data */
    void updateSampleDescs(std::vector<SampleDesc>* sampleDescs);
    
    /** Export all loaded samples to WAV files in the specified directory */
    bool exportAllSamples(const juce::String& outputDirectory);
    
    /** Get all loaded sample names */
    std::vector<juce::String> getAllSampleNames() const;
    
private:
    //==============================================================================
    std::map<juce::String, std::unique_ptr<AudioFile<float>>> loadedSamples;
    juce::String samplerDirectoryPath;
    
    juce::String expandPath(const juce::String& path);
    juce::String getFullPath(const juce::String& sampleName, const juce::String& samplerDirectory);
};

