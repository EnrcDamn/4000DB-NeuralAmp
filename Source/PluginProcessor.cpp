/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ModelData.h"

//==============================================================================
NeuralAmpAudioProcessor::NeuralAmpAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
     , parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
#else
     : parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    gainParameter = parameters.getRawParameterValue ("gain");
    jassert (gainParameter != nullptr);

    for (auto& model : neuralModels)
    {
        const auto loaded = model.loadModel (
            ModelData::model_best_json,
            static_cast<std::size_t> (ModelData::model_best_jsonSize));
        jassertquiet (loaded);
        juce::ignoreUnused (loaded);
    }
}

NeuralAmpAudioProcessor::~NeuralAmpAudioProcessor()
{
}

//==============================================================================
const juce::String NeuralAmpAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NeuralAmpAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NeuralAmpAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NeuralAmpAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NeuralAmpAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NeuralAmpAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NeuralAmpAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NeuralAmpAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NeuralAmpAudioProcessor::getProgramName (int index)
{
    return {};
}

void NeuralAmpAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NeuralAmpAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    for (auto& model : neuralModels)
    {
        model.reset();
    }

    // 20 ms linear smoothing
    m_smoothedGain.reset(sampleRate, 0.02);
    m_smoothedGain.setCurrentAndTargetValue(gainParameter->load());
}

void NeuralAmpAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NeuralAmpAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void NeuralAmpAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto channelsToProcess = juce::jmin(totalNumInputChannels, static_cast<int>(neuralModels.size()));

    for (auto channel = totalNumInputChannels; channel < totalNumOutputChannels; ++channel)
    {
        buffer.clear(channel, 0, buffer.getNumSamples());
    }

    m_smoothedGain.setTargetValue(gainParameter->load());
    auto channelData = buffer.getArrayOfWritePointers();

    for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const auto gain = m_smoothedGain.getNextValue();

        for (auto channel = 0; channel < channelsToProcess; ++channel)
        {
            // Replace input with neural network output
            channelData[channel][sample] = neuralModels[static_cast<std::size_t>(channel)]
                                               .processSample(channelData[channel][sample], gain);
        }
    }
}

//==============================================================================
bool NeuralAmpAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NeuralAmpAudioProcessor::createEditor()
{
    return new NeuralAmpAudioProcessorEditor (*this);
}

//==============================================================================
void NeuralAmpAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void NeuralAmpAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorValueTreeState::ParameterLayout NeuralAmpAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ "gain", 1 },
        "Gain",
        juce::NormalisableRange<float>{ 0.0f, 1.0f, 0.01f },
        0.0f));
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NeuralAmpAudioProcessor();
}
