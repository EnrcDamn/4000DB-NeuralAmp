/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TransparentLookAndFeel.h"

//==============================================================================
/**
*/
class NeuralAmpAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NeuralAmpAudioProcessorEditor (NeuralAmpAudioProcessor&);
    ~NeuralAmpAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NeuralAmpAudioProcessor& audioProcessor;
    juce::Image backgroundImage;

    juce::Slider gainKnob;
    TransparentLookAndFeel lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeuralAmpAudioProcessorEditor)
};
