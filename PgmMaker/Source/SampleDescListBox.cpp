#include "SampleDescListBox.h"

//==============================================================================
SampleDescListBox::SampleDescListBox()
{
    tableListBox.setModel(&model);
    setupTable();
    addAndMakeVisible(tableListBox);
}

void SampleDescListBox::setupTable()
{
    tableListBox.getHeader().addColumn("Name", SampleDescListModel::Name, 200, 50, 400);
    tableListBox.getHeader().addColumn("Size", SampleDescListModel::Size, 120, 50, 300);
    tableListBox.getHeader().addColumn("Channels", SampleDescListModel::Channels, 80, 50, 150);
    tableListBox.getHeader().addColumn("Sample Rate", SampleDescListModel::SampleRate, 120, 50, 200);
    tableListBox.getHeader().addColumn("Play", SampleDescListModel::Play, 80, 50, 150);
}

void SampleDescListBox::setSampleDescs(std::vector<SampleDesc>* sampleDescs)
{
    model.setSampleDescs(sampleDescs);
    tableListBox.updateContent();
}

void SampleDescListBox::setPlayCallback(std::function<bool(const juce::String&)> callback)
{
    model.setPlayCallback(callback);
}

void SampleDescListBox::updateButtonStates(const juce::String& currentlyPlayingSample)
{
    model.updateButtonStates(currentlyPlayingSample);
}

void SampleDescListBox::updateContent()
{
    tableListBox.updateContent();
}

void SampleDescListBox::resized()
{
    tableListBox.setBounds(getLocalBounds());
}

