/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "BinaryData.h"

//==============================================================================
NeuralAmpAudioProcessorEditor::NeuralAmpAudioProcessorEditor (NeuralAmpAudioProcessor& p)
    : AudioProcessorEditor (&p),
      m_audioProcessor (p),
      m_gainAttachment (p.parameters, "gain", m_gainKnob)
{
    // 4:3 aspect ratio
    setSize (640, 480);

    m_backgroundImage = juce::ImageCache::getFromMemory(
        BinaryData::akai4000db_background_png,
        BinaryData::akai4000db_background_pngSize
    );

    m_gainKnob.setSliderStyle(juce::Slider::Rotary);
    m_gainKnob.setRange(0.0, 1.0, 0.01);
    m_gainKnob.setNumDecimalPlacesToDisplay(2);
    // Fixed width keeps every displayed value within the same compact field.
    m_gainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 36, 22);
    m_gainKnob.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    m_gainKnob.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    m_gainKnob.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

    m_gainKnob.setLookAndFeel(&m_lookAndFeel); // override drawRotarySlsider to draw nothing
    addAndMakeVisible(m_gainKnob);
}

NeuralAmpAudioProcessorEditor::~NeuralAmpAudioProcessorEditor()
{
    m_gainKnob.setLookAndFeel(nullptr);
}

//==============================================================================
void NeuralAmpAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.drawImageWithin(
        m_backgroundImage,
        0,
        0,
        getWidth(),
        getHeight(),
        juce::RectanglePlacement::stretchToFit
    );
}

void NeuralAmpAudioProcessorEditor::resized()
{
    // Real ratio is image height 767 px, agains 111 px of knob radius -> ca. 0.14444
    auto const knobRadius = static_cast<int>(getWidth() * 0.14444);

    m_gainKnob.setBounds(
        getWidth() / 2 - knobRadius,
        getHeight()  * 11 / 15 - knobRadius,
        knobRadius * 2,
        knobRadius * 2 + 22
    );
}
