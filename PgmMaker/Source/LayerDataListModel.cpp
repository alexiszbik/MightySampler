#include "LayerDataListModel.h"

//==============================================================================
LayerDataListModel::LayerDataListModel(std::vector<LayerData>* layers)
    : layers(layers)
{
}

int LayerDataListModel::getNumRows()
{
    if (layers == nullptr)
        return 0;
    
    return static_cast<int>(layers->size());
}

void LayerDataListModel::paintListBoxItem(int rowNumber, juce::Graphics& g, 
                                           int width, int height, bool rowIsSelected)
{
    if (layers == nullptr || rowNumber < 0 || rowNumber >= static_cast<int>(layers->size()))
        return;
    
    const auto& layer = (*layers)[rowNumber];
    
    // Background color based on selection
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else
        g.fillAll((rowNumber % 2 == 0) ? juce::Colours::white : juce::Colours::lightgrey);
    
    g.setColour(juce::Colours::black);
    
    // Draw layer information
    auto area = juce::Rectangle<int>(5, 0, width - 10, height);
    auto font = g.getCurrentFont();
    
    // Sample ID
    g.drawText("Sample ID: " + juce::String(layer.sampleId), 
               area.removeFromTop(height / 2), 
               juce::Justification::centredLeft);
    
    // Play Mode, Reverse, and Pitch
    juce::String info = "Mode: " + getPlayModeString(layer.playMode);
    info += " | Reverse: " + juce::String(layer.isReverse ? "Yes" : "No");
    info += " | Pitch: " + juce::String(layer.pitch);
    
    g.drawText(info, area, juce::Justification::centredLeft);
}

void LayerDataListModel::setLayers(std::vector<LayerData>* newLayers)
{
    layers = newLayers;
}

juce::String LayerDataListModel::getPlayModeString(EPlayMode mode)
{
    switch (mode)
    {
        case Trigger: return "Trigger";
        case Gate: return "Gate";
        case OneShot: return "OneShot";
        default: return "Unknown";
    }
}

