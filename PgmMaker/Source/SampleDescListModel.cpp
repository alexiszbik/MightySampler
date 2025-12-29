#include "SampleDescListModel.h"

//==============================================================================
PlayButtonComponent::PlayButtonComponent()
{
    playButton.setButtonText("Play");
    playButton.addListener(this);
    addAndMakeVisible(playButton);
}

void PlayButtonComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.reduce(5, 2);
    playButton.setBounds(bounds);
}

void PlayButtonComponent::buttonClicked(juce::Button* button)
{
    if (button == &playButton && sampleDesc != nullptr && playCallback)
    {
        juce::String sampleName(sampleDesc->sampleName);
        bool isNowPlaying = playCallback(sampleName);
        updateButtonState(isNowPlaying);
    }
}

void PlayButtonComponent::setSampleDesc(const SampleDesc* desc)
{
    sampleDesc = desc;
    currentlyPlaying = false;
    updateButtonState(false);
}

void PlayButtonComponent::setPlayCallback(std::function<bool(const juce::String&)> callback)
{
    playCallback = callback;
}

void PlayButtonComponent::updateButtonState(bool isPlaying)
{
    currentlyPlaying = isPlaying;
    playButton.setButtonText(isPlaying ? "Stop" : "Play");
}

juce::String PlayButtonComponent::getSampleName() const
{
    if (sampleDesc != nullptr)
        return juce::String(sampleDesc->sampleName);
    return juce::String();
}

//==============================================================================
SampleDescListModel::SampleDescListModel()
{
}

int SampleDescListModel::getNumRows()
{
    if (sampleDescs == nullptr)
        return 0;
    
    return static_cast<int>(sampleDescs->size());
}

void SampleDescListModel::paintRowBackground(juce::Graphics& g, int rowNumber, 
                                             int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else
        g.fillAll((rowNumber % 2 == 0) ? juce::Colours::white : juce::Colours::lightgrey);
}

void SampleDescListModel::paintCell(juce::Graphics& g, int rowNumber, int columnId,
                                    int width, int height, bool rowIsSelected)
{
    if (sampleDescs == nullptr || rowNumber < 0 || rowNumber >= static_cast<int>(sampleDescs->size()))
        return;
    
    const auto& sampleDesc = (*sampleDescs)[rowNumber];
    g.setColour(juce::Colours::black);
    
    juce::Rectangle<int> area(0, 0, width, height);
    area.removeFromLeft(5);
    
    switch (columnId)
    {
        case Name:
            g.drawText(juce::String(sampleDesc.sampleName), area, juce::Justification::centredLeft);
            break;
        case Size:
            g.drawText(juce::String(sampleDesc.sampleData.sampleSize) + " bytes", area, juce::Justification::centredLeft);
            break;
        case Channels:
            g.drawText(juce::String(sampleDesc.sampleData.sampleChanCount), area, juce::Justification::centredLeft);
            break;
        case SampleRate:
            g.drawText(juce::String(sampleDesc.sampleData.sampleRate, 0) + " Hz", area, juce::Justification::centredLeft);
            break;
        case Play:
            // Play button is handled by refreshComponentForCell
            break;
    }
}

juce::Component* SampleDescListModel::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                                               juce::Component* existingComponentToUpdate)
{
    if (columnId != Play)
    {
        if (existingComponentToUpdate != nullptr)
            delete existingComponentToUpdate;
        return nullptr;
    }
    
    if (sampleDescs == nullptr || rowNumber < 0 || rowNumber >= static_cast<int>(sampleDescs->size()))
    {
        if (existingComponentToUpdate != nullptr)
            delete existingComponentToUpdate;
        return nullptr;
    }
    
    PlayButtonComponent* component = nullptr;
    
    if (existingComponentToUpdate != nullptr)
    {
        component = dynamic_cast<PlayButtonComponent*>(existingComponentToUpdate);
    }
    
    if (component == nullptr)
    {
        component = new PlayButtonComponent();
        playButtonComponents.add(component);
    }
    
    component->setSampleDesc(&(*sampleDescs)[rowNumber]);
    component->setPlayCallback(playCallback);
    
    return component;
}

void SampleDescListModel::setSampleDescs(std::vector<SampleDesc>* newSampleDescs)
{
    sampleDescs = newSampleDescs;
    playButtonComponents.clear();
}

void SampleDescListModel::setPlayCallback(std::function<bool(const juce::String&)> callback)
{
    playCallback = callback;
    // Update all existing button components
    for (auto* component : playButtonComponents)
    {
        component->setPlayCallback(playCallback);
    }
}

void SampleDescListModel::updateButtonStates(const juce::String& currentlyPlayingSample)
{
    for (auto* component : playButtonComponents)
    {
        juce::String sampleName = component->getSampleName();
        bool isPlaying = (sampleName == currentlyPlayingSample && !sampleName.isEmpty());
        component->updateButtonState(isPlaying);
    }
}

