#include "SampleDescListBox.h"

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
    if (sampleDescs == nullptr || rowNumber < 0 || rowNumber >= static_cast<int>(sampleDescs->size()))
        return;
    
    const auto& sampleDesc = (*sampleDescs)[rowNumber];
    
    // Background color based on selection
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else
        g.fillAll((rowNumber % 2 == 0) ? juce::Colours::white : juce::Colours::lightgrey);
    
    g.setColour(juce::Colours::black);
    
    // Draw sample information
    auto area = juce::Rectangle<int>(5, 0, width - 10, height);
    auto font = g.getCurrentFont();
    
    // Sample Name
    juce::String sampleName(sampleDesc.sampleName);
    g.drawText("Name: " + sampleName, 
               area.removeFromTop(height / 3), 
               juce::Justification::centredLeft);
    
    // Sample Data info
    juce::String info = "Size: " + juce::String(sampleDesc.sampleData.sampleSize) + " bytes";
    info += " | Channels: " + juce::String(sampleDesc.sampleData.sampleChanCount);
    info += " | Sample Rate: " + juce::String(sampleDesc.sampleData.sampleRate, 0) + " Hz";
    
    g.drawText(info, area, juce::Justification::centredLeft);
}

void SampleDescListModel::setSampleDescs(std::vector<SampleDesc>* newSampleDescs)
{
    sampleDescs = newSampleDescs;
}

//==============================================================================
SampleDescListBox::SampleDescListBox()
    : model(nullptr)
{
    listBox.setModel(&model);
    addAndMakeVisible(listBox);
}

void SampleDescListBox::setSampleDescs(std::vector<SampleDesc>* sampleDescs)
{
    model.setSampleDescs(sampleDescs);
    listBox.updateContent();
}

void SampleDescListBox::updateContent()
{
    listBox.updateContent();
}

void SampleDescListBox::resized()
{
    listBox.setBounds(getLocalBounds());
}

