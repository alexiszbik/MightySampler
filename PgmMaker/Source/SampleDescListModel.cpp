#include "SampleDescListModel.h"

//==============================================================================
SampleDescRowComponent::SampleDescRowComponent()
{
    playButton.setButtonText("Play");
    playButton.addListener(this);
    addAndMakeVisible(playButton);
}

void SampleDescRowComponent::paint(juce::Graphics& g)
{
    if (sampleDesc == nullptr)
        return;
    
    // Background color based on selection
    if (isSelected)
        g.fillAll(juce::Colours::lightblue);
    else
        g.fillAll((rowNumber % 2 == 0) ? juce::Colours::white : juce::Colours::lightgrey);
    
    g.setColour(juce::Colours::black);
    
    // Draw sample information
    auto area = getLocalBounds();
    area.removeFromLeft(5);
    area.removeFromRight(5);
    
    // Reserve space for play button
    auto buttonArea = area.removeFromRight(80);
    area.removeFromRight(5);
    
    // Sample Name
    juce::String sampleName(sampleDesc->sampleName);
    auto nameArea = area.removeFromTop(getHeight() / 3);
    g.drawText("Name: " + sampleName, nameArea, juce::Justification::centredLeft);
    
    // Sample Data info
    juce::String info = "Size: " + juce::String(sampleDesc->sampleData.sampleSize) + " bytes";
    info += " | Channels: " + juce::String(sampleDesc->sampleData.sampleChanCount);
    info += " | Sample Rate: " + juce::String(sampleDesc->sampleData.sampleRate, 0) + " Hz";
    
    g.drawText(info, area, juce::Justification::centredLeft);
}

void SampleDescRowComponent::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromRight(5);
    playButton.setBounds(bounds.removeFromRight(75));
}

void SampleDescRowComponent::buttonClicked(juce::Button* button)
{
    if (button == &playButton && sampleDesc != nullptr)
    {
        // TODO: Implement play functionality
        juce::String sampleName(sampleDesc->sampleName);
        std::cout << "Playing sample: " << sampleName << std::endl;
    }
}

void SampleDescRowComponent::setSampleDesc(const SampleDesc* desc, int row, bool selected)
{
    sampleDesc = desc;
    rowNumber = row;
    isSelected = selected;
    repaint();
}

//==============================================================================
SampleDescListModel::SampleDescListModel(std::vector<SampleDesc>* sampleDescs)
    : sampleDescs(sampleDescs)
{
}

int SampleDescListModel::getNumRows()
{
    if (sampleDescs == nullptr)
        return 0;
    
    return static_cast<int>(sampleDescs->size());
}

void SampleDescListModel::paintListBoxItem(int rowNumber, juce::Graphics& g, 
                                           int width, int height, bool rowIsSelected)
{
    // This method is not used when using custom components
    // But we keep it for fallback
}

juce::Component* SampleDescListModel::refreshComponentForRow(int rowNumber, bool isRowSelected,
                                                             juce::Component* existingComponentToUpdate)
{
    if (sampleDescs == nullptr || rowNumber < 0 || rowNumber >= static_cast<int>(sampleDescs->size()))
    {
        if (existingComponentToUpdate != nullptr)
            delete existingComponentToUpdate;
        return nullptr;
    }
    
    SampleDescRowComponent* component = nullptr;
    
    if (existingComponentToUpdate != nullptr)
    {
        component = dynamic_cast<SampleDescRowComponent*>(existingComponentToUpdate);
    }
    
    if (component == nullptr)
    {
        component = new SampleDescRowComponent();
        components.add(component);
    }
    
    component->setSampleDesc(&(*sampleDescs)[rowNumber], rowNumber, isRowSelected);
    
    return component;
}

int SampleDescListModel::getRowHeight(int rowNumber)
{
    return 60; // Fixed height for each row
}

void SampleDescListModel::setSampleDescs(std::vector<SampleDesc>* newSampleDescs)
{
    sampleDescs = newSampleDescs;
    components.clear();
}

