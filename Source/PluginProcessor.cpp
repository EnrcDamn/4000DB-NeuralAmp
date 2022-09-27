/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NeuralReelSaturatorAudioProcessor::NeuralReelSaturatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                     treeState(*this,
                                nullptr,
                                "PARAMETER",
                                { std::make_unique<AudioParameterFloat>(GAIN_ID, GAIN_NAME, NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f) })
#endif
{
    gainParam = treeState.getRawParameterValue (GAIN_ID);
}

NeuralReelSaturatorAudioProcessor::~NeuralReelSaturatorAudioProcessor()
{
}

//==============================================================================
const juce::String NeuralReelSaturatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NeuralReelSaturatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NeuralReelSaturatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NeuralReelSaturatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NeuralReelSaturatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NeuralReelSaturatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NeuralReelSaturatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NeuralReelSaturatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NeuralReelSaturatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void NeuralReelSaturatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NeuralReelSaturatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lstmL.reset();
    lstmR.reset();

    lstmL.load_model();
    lstmR.load_model();
}

void NeuralReelSaturatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NeuralReelSaturatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NeuralReelSaturatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    const int numSamples = buffer.getNumSamples();

    auto gainValue = static_cast<float> (gainParam->load());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        if (channel == 0)
            lstmL.process(buffer.getWritePointer(0), gainValue, buffer.getWritePointer(0), (int)numSamples);
        else if (channel == 1)
            lstmL.process(buffer.getWritePointer(1), gainValue, buffer.getWritePointer(1), (int)numSamples);
    }
}

//==============================================================================
bool NeuralReelSaturatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NeuralReelSaturatorAudioProcessor::createEditor()
{
    return new NeuralReelSaturatorAudioProcessorEditor (*this);
}

//==============================================================================
void NeuralReelSaturatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NeuralReelSaturatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NeuralReelSaturatorAudioProcessor();
}
