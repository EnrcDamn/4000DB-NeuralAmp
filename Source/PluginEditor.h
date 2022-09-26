/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NeuralReelSaturatorAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                 public juce::Slider::Listener
{
public:
    NeuralReelSaturatorAudioProcessorEditor (NeuralReelSaturatorAudioProcessor&);
    ~NeuralReelSaturatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

    juce::ScopedPointer <juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttach;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NeuralReelSaturatorAudioProcessor& audioProcessor;
    juce::Slider gainSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeuralReelSaturatorAudioProcessorEditor)
};
