#include "LayerDataListModel.h"

//==============================================================================
LayerDataListModel::LayerDataListModel()
{
}

int LayerDataListModel::getNumRows()
{
    if (layers == nullptr)
        return 0;
    
    return static_cast<int>(layers->size());
}

void LayerDataListModel::paintRowBackground(juce::Graphics& g, int rowNumber, 
                                            int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::lightblue);
    else
        g.fillAll((rowNumber % 2 == 0) ? juce::Colours::white : juce::Colours::lightgrey);
}

void LayerDataListModel::paintCell(juce::Graphics& g, int rowNumber, int columnId,
                                   int width, int height, bool rowIsSelected)
{
    if (layers == nullptr || rowNumber < 0 || rowNumber >= static_cast<int>(layers->size()))
        return;
    
    const auto& layer = (*layers)[rowNumber];
    g.setColour(juce::Colours::black);
    
    juce::Rectangle<int> area(0, 0, width, height);
    area.removeFromLeft(5);
    
    switch (columnId)
    {
        case LayerId:
            g.drawText(juce::String(rowNumber), area, juce::Justification::centredLeft);
            break;
        case SampleId: {
            auto sampleStr = juce::String(sampleDescs->at(layer.sampleId).sampleName);
            g.drawText(sampleStr, area, juce::Justification::centredLeft);
        }
            break;
        case PlayMode:
            g.drawText(getPlayModeString(layer.playMode), area, juce::Justification::centredLeft);
            break;
        case Reverse:
            g.drawText(layer.isReverse ? "Yes" : "No", area, juce::Justification::centredLeft);
            break;
        case Pitch:
            g.drawText(juce::String(layer.pitch), area, juce::Justification::centredLeft);
            break;
    }
}

juce::Component* LayerDataListModel::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                                              juce::Component* existingComponentToUpdate)
{
    // No custom components needed for LayerData
    if (existingComponentToUpdate != nullptr)
        delete existingComponentToUpdate;
    return nullptr;
}

void LayerDataListModel::setLayers(std::vector<LayerData>* newLayers) {
    layers = newLayers;
}

void LayerDataListModel::setSampleDescs(std::vector<SampleDesc>* newSampleDescs) {
    sampleDescs = newSampleDescs;
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

