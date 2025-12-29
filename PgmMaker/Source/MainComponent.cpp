#include "MainComponent.h"
#include "../../PatchReader.h"

//==============================================================================
MainComponent::MainComponent()
    : tabbedComponent(juce::TabbedButtonBar::TabsAtTop)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    
    tabbedComponent.addTab("Samples", juce::Colours::lightgrey, &sampleDescListBox, false);
    tabbedComponent.addTab("Layers", juce::Colours::lightgrey, &layerDataListBox, false);
    
    addAndMakeVisible(tabbedComponent);
    
    auto docsDir = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory);
    auto samplerDir = docsDir.getChildFile ("sampler");
    
    if (samplerDir.isDirectory())
    {
        auto patchFile = samplerDir.getChildFile ("file.ymnk");
        if (patchFile.exists()) {

            juce::StringArray patchLines;
            patchFile.readLines(patchLines);
            for (auto& line : patchLines) {
                if (!patchReader.read(line.toUTF8())) {
                    std::cout << "aouch" << std::endl;
                }
            }
            
            // Load all samples into memory
            juce::String samplerPath = samplerDir.getFullPathName();
            
            if (sampleManager.loadAllSamples(samplerPath))
            {
                std::cout << "SampleManager: Successfully loaded " << sampleManager.getNumLoadedSamples() << " samples" << std::endl;
            }
            else
            {
                std::cout << "SampleManager: Some samples failed to load" << std::endl;
            }
            
            // Update sample descriptions with loaded audio data
            sampleManager.updateSampleDescs(&patchReader.sampleDescs);
            
            // Update the list boxes with the loaded data
            layerDataListBox.setData(&patchReader.layers, &patchReader.sampleDescs);
            sampleDescListBox.setSampleDescs(&patchReader.sampleDescs);
            
            // Set up the play callback
            sampleDescListBox.setPlayCallback([this](const juce::String& sampleName) {
                return onPlaySample(sampleName);
            });
        }

    } else {
        
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    
    samplePlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    // Set callback for when playback stops
    samplePlayer.setPlaybackStoppedCallback([this]() {
        juce::MessageManager::callAsync([this]() {
            currentlyPlayingSample = juce::String();
            sampleDescListBox.updateButtonStates(juce::String());
        });
    });
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()
    
    // Play the sample if one is playing
    samplePlayer.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    
    samplePlayer.releaseResources();
}

//==============================================================================
bool MainComponent::onPlaySample(const juce::String& sampleName)
{
    // If clicking on the same sample that's playing, stop it
    if (currentlyPlayingSample == sampleName && samplePlayer.isPlaying())
    {
        samplePlayer.stop();
        currentlyPlayingSample = juce::String();
        sampleDescListBox.updateButtonStates(juce::String());
        std::cout << "Stopped sample: " << sampleName << std::endl;
        return false;
    }
    
    // Stop current sample if playing a different one
    if (samplePlayer.isPlaying())
    {
        samplePlayer.stop();
    }
    
    AudioFile<float>* audioFile = sampleManager.getSample(sampleName);
    
    if (audioFile != nullptr)
    {
        samplePlayer.setSample(audioFile);
        samplePlayer.start();
        currentlyPlayingSample = sampleName;
        sampleDescListBox.updateButtonStates(sampleName);
        std::cout << "Playing sample: " << sampleName << std::endl;
        return true;
    }
    else
    {
        std::cout << "Sample not found: " << sampleName << std::endl;
        currentlyPlayingSample = juce::String();
        sampleDescListBox.updateButtonStates(juce::String());
        return false;
    }
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    auto bounds = getLocalBounds();
    tabbedComponent.setBounds(bounds);
}
