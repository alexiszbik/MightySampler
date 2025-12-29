#pragma once

#include <JuceHeader.h>
#include "../../PatchReader.h"

//==============================================================================
/*
    Model class for displaying LayerData items in a TableListBox
*/
class LayerDataListModel : public juce::TableListBoxModel
{
public:
    //==============================================================================
    LayerDataListModel();
    
    //==============================================================================
    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                            juce::Component* existingComponentToUpdate) override;
    
    void setLayers(std::vector<LayerData>* layers);
    void setSampleDescs(std::vector<SampleDesc>* sampleDescs);
    
    enum ColumnIds
    {
        LayerId = 1,
        SampleId,
        PlayMode,
        Reverse,
        Pitch
    };
    
private:
    //==============================================================================
    std::vector<LayerData>* layers = nullptr;
    std::vector<SampleDesc>* sampleDescs = nullptr;
    
    juce::String getPlayModeString(EPlayMode mode);
};

