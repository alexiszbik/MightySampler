#include "LayerDataListBox.h"

//==============================================================================
LayerDataListBox::LayerDataListBox()
{
    tableListBox.setModel(&model);
    setupTable();
    addAndMakeVisible(tableListBox);
}

void LayerDataListBox::setupTable()
{
    tableListBox.getHeader().addColumn("Layer ID", LayerDataListModel::LayerId, 50, 50, 50);
    tableListBox.getHeader().addColumn("Sample ID", LayerDataListModel::SampleId, 100, 50, 200);
    tableListBox.getHeader().addColumn("Play Mode", LayerDataListModel::PlayMode, 120, 50, 200);
    tableListBox.getHeader().addColumn("Reverse", LayerDataListModel::Reverse, 80, 50, 200);
    tableListBox.getHeader().addColumn("Pitch", LayerDataListModel::Pitch, 80, 50, 200);
}

void LayerDataListBox::setData(std::vector<LayerData>* layers, std::vector<SampleDesc>* sampleDescs)
{
    model.setLayers(layers);
    model.setSampleDescs(sampleDescs);
    tableListBox.updateContent();
}

void LayerDataListBox::updateContent()
{
    tableListBox.updateContent();
}

void LayerDataListBox::resized()
{
    tableListBox.setBounds(getLocalBounds());
}

