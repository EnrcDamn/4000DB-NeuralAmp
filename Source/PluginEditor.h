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
class NeuralAmpAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    NeuralAmpAudioProcessorEditor (NeuralAmpAudioProcessor&);
    ~NeuralAmpAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawVuNeedle(juce::Graphics&);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NeuralAmpAudioProcessor& m_audioProcessor;
    juce::Image m_backgroundImage;

    juce::Slider m_gainKnob;
    juce::AudioProcessorValueTreeState::SliderAttachment m_gainAttachment;
    TransparentLookAndFeel m_lookAndFeel;
    float m_meterDb = -20.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeuralAmpAudioProcessorEditor)
};
