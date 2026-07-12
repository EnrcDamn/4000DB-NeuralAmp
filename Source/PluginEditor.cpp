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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);

    backgroundImage = juce::ImageCache::getFromMemory(
        BinaryData::akai4000db_background_png,
        BinaryData::akai4000db_background_pngSize
    );

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
}
