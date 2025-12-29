#include "SampleManager.h"

//==============================================================================
SampleManager::SampleManager()
{
}

SampleManager::~SampleManager()
{
    clear();
}

bool SampleManager::loadAllSamples(const juce::String& samplerDirectory)
{
    samplerDirectoryPath = expandPath(samplerDirectory);
    juce::File samplerDir(samplerDirectoryPath);
    
    if (!samplerDir.isDirectory())
    {
        std::cout << "SampleManager: Directory does not exist: " << samplerDirectoryPath << std::endl;
        return false;
    }
    
    juce::Array<juce::File> audioFiles;
    samplerDir.findChildFiles(audioFiles, juce::File::findFiles, false, "*.wav");
    samplerDir.findChildFiles(audioFiles, juce::File::findFiles, false, "*.WAV");
    
    bool allLoaded = true;
    
    for (const auto& file : audioFiles)
    {
        juce::String fileName = file.getFileName();
        juce::String fullPath = file.getFullPathName();
        
        if (loadedSamples.find(fileName) == loadedSamples.end())
        {
            auto audioFile = std::make_unique<AudioFile<float>>();
            
            if (audioFile->load(fullPath.toStdString()))
            {
                loadedSamples[fileName] = std::move(audioFile);
                std::cout << "SampleManager: Loaded " << fileName << std::endl;
            }
            else
            {
                std::cout << "SampleManager: Failed to load " << fileName << std::endl;
                allLoaded = false;
            }
        }
    }
    
    return allLoaded;
}

bool SampleManager::loadSample(const juce::String& sampleName, const juce::String& samplerDirectory)
{
    if (isSampleLoaded(sampleName))
    {
        return true; // Already loaded
    }
    
    juce::String fullPath = getFullPath(sampleName, samplerDirectory);
    juce::File file(fullPath);
    
    if (!file.existsAsFile())
    {
        std::cout << "SampleManager: File does not exist: " << fullPath << std::endl;
        return false;
    }
    
    auto audioFile = std::make_unique<AudioFile<float>>();
    
    if (audioFile->load(fullPath.toStdString()))
    {
        loadedSamples[sampleName] = std::move(audioFile);
        std::cout << "SampleManager: Loaded " << sampleName << std::endl;
        return true;
    }
    else
    {
        std::cout << "SampleManager: Failed to load " << sampleName << std::endl;
        return false;
    }
}

AudioFile<float>* SampleManager::getSample(const juce::String& sampleName)
{
    auto it = loadedSamples.find(sampleName);
    if (it != loadedSamples.end())
    {
        return it->second.get();
    }
    return nullptr;
}

AudioFile<float>* SampleManager::getSample(size_t index)
{
    if (index >= loadedSamples.size())
        return nullptr;
    
    auto it = loadedSamples.begin();
    std::advance(it, index);
    return it->second.get();
}

bool SampleManager::isSampleLoaded(const juce::String& sampleName)
{
    return loadedSamples.find(sampleName) != loadedSamples.end();
}

size_t SampleManager::getNumLoadedSamples() const
{
    return loadedSamples.size();
}

void SampleManager::clear()
{
    loadedSamples.clear();
}

void SampleManager::updateSampleDescs(std::vector<SampleDesc>* sampleDescs)
{
    if (sampleDescs == nullptr)
        return;
    
    for (auto& sampleDesc : *sampleDescs)
    {
        juce::String sampleName(sampleDesc.sampleName);
        AudioFile<float>* audioFile = getSample(sampleName);
        
        if (audioFile != nullptr)
        {
            // Update SampleDesc with loaded audio data
            sampleDesc.sampleData.sampleSize = audioFile->getNumSamplesPerChannel();
            sampleDesc.sampleData.sampleChanCount = audioFile->getNumChannels();
            sampleDesc.sampleData.sampleRate = static_cast<double>(audioFile->getSampleRate());
            
            // Note: AudioFile stores samples in a different format (vector of vectors)
            // The data pointer in SampleData would need to be handled differently
            // if you want to use the raw int16_t* data pointer
        }
    }
}

juce::String SampleManager::expandPath(const juce::String& path)
{
    juce::String expanded = path;
    
    // Expand ~ to home directory
    if (expanded.startsWith("~/"))
    {
        juce::File homeDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory);
        expanded = homeDir.getFullPathName() + expanded.substring(1);
    }
    
    return expanded;
}

juce::String SampleManager::getFullPath(const juce::String& sampleName, const juce::String& samplerDirectory)
{
    juce::String dirPath = expandPath(samplerDirectory);
    return juce::File(dirPath).getChildFile(sampleName).getFullPathName();
}

bool SampleManager::exportAllSamples(const juce::String& outputDirectory)
{
    juce::File outputDir(expandPath(outputDirectory));
    
    if (!outputDir.isDirectory())
    {
        if (!outputDir.createDirectory())
        {
            std::cout << "SampleManager: Failed to create output directory: " << outputDirectory << std::endl;
            return false;
        }
    }
    
    bool allExported = true;
    
    for (const auto& pair : loadedSamples)
    {
        const juce::String& sampleName = pair.first;
        AudioFile<float>* audioFile = pair.second.get();
        
        if (audioFile == nullptr)
            continue;
        
        // Ensure the filename ends with .wav
        juce::String fileName = sampleName;
        if (!fileName.endsWithIgnoreCase(".wav"))
        {
            // Remove any existing extension and add .wav
            int lastDot = fileName.lastIndexOfChar('.');
            if (lastDot > 0)
                fileName = fileName.substring(0, lastDot);
            fileName += ".wav";
        }
        
        juce::File outputFile = outputDir.getChildFile(fileName);
        std::string filePath = outputFile.getFullPathName().toStdString();
        
        // Create a 16-bit version of the audio file
        AudioFile<int16_t> audioFile16Bit;
        
        // Copy audio buffer from float to int16_t
        int numChannels = audioFile->getNumChannels();
        int numSamples = audioFile->getNumSamplesPerChannel();
        
        audioFile16Bit.setNumChannels(numChannels);
        audioFile16Bit.setNumSamplesPerChannel(numSamples);
        audioFile16Bit.setSampleRate(audioFile->getSampleRate());
        audioFile16Bit.setBitDepth(16);
        
        // Convert float samples to int16_t
        for (int channel = 0; channel < numChannels; channel++)
        {
            for (int i = 0; i < numSamples; i++)
            {
                float sample = audioFile->samples[channel][i];
                // Clamp to [-1.0, 1.0] range
                sample = std::max(-1.0f, std::min(1.0f, sample));
                // Convert to 16-bit integer
                audioFile16Bit.samples[channel][i] = static_cast<int16_t>(sample * 32767.0f);
            }
        }
        
        if (audioFile16Bit.save(filePath))
        {
            std::cout << "SampleManager: Exported " << fileName << " (16-bit)" << std::endl;
        }
        else
        {
            std::cout << "SampleManager: Failed to export " << fileName << std::endl;
            allExported = false;
        }
    }
    
    return allExported;
}

std::vector<juce::String> SampleManager::getAllSampleNames() const
{
    std::vector<juce::String> names;
    for (const auto& pair : loadedSamples)
    {
        names.push_back(pair.first);
    }
    return names;
}

