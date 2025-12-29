#include "LayerDataListBox.h"

//==============================================================================
LayerDataListBox::LayerDataListBox()
    : model(nullptr)
{
    listBox.setModel(&model);
    addAndMakeVisible(listBox);
}

void LayerDataListBox::setLayers(std::vector<LayerData>* layers)
{
    model.setLayers(layers);
    listBox.updateContent();
}

void LayerDataListBox::updateContent()
{
    listBox.updateContent();
}

void LayerDataListBox::resized()
{
    listBox.setBounds(getLocalBounds());
}

