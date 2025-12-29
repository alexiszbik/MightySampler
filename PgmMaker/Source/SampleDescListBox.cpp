#include "SampleDescListBox.h"

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

