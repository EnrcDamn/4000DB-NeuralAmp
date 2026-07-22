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
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // 4:3 aspect ratio
    setSize (640, 480);

    backgroundImage = juce::ImageCache::getFromMemory(
        BinaryData::akai4000db_background_png,
        BinaryData::akai4000db_background_pngSize
    );

    gainKnob.setSliderStyle(juce::Slider::Rotary);
    gainKnob.setRange(0.0, 1.0, 0.01);
    gainKnob.setNumDecimalPlacesToDisplay(2);
    // Fixed width keeps every displayed value within the same compact field.
    gainKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 36, 22);
    gainKnob.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    gainKnob.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    gainKnob.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

    gainKnob.setLookAndFeel(&lookAndFeel); // override drawRotarySlsider to draw nothing
    addAndMakeVisible(gainKnob);
}

NeuralAmpAudioProcessorEditor::~NeuralAmpAudioProcessorEditor()
{
}

//==============================================================================
void NeuralAmpAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.drawImageWithin(
        backgroundImage,
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

    gainKnob.setBounds(
        getWidth() / 2 - knobRadius,
        getHeight()  * 11 / 15 - knobRadius,
        knobRadius * 2,
        knobRadius * 2 + 22
    );
}
