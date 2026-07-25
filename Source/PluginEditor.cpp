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

    startTimerHz(60);
}

NeuralAmpAudioProcessorEditor::~NeuralAmpAudioProcessorEditor()
{
    stopTimer();
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

    drawVuNeedle(g);
}

void NeuralAmpAudioProcessorEditor::drawVuNeedle(juce::Graphics& g)
{
    constexpr auto minimumDb = -20.0f;
    constexpr auto maximumDb = 3.0f;
    constexpr auto startAngle = -2.64f; // −151 degrees
    constexpr auto endAngle = -0.52f;   // −30 degrees

    // The face is a true VU-style scale: angular deflection is proportional
    // to amplitude, rather than directly proportional to decibels.
    auto const minimumAmplitude = juce::Decibels::decibelsToGain(minimumDb);
    auto const maximumAmplitude = juce::Decibels::decibelsToGain(maximumDb);
    auto const meterAmplitude = juce::Decibels::decibelsToGain(juce::jlimit(minimumDb, maximumDb, m_meterDb));

    // Normalize position: −20 VU -> 0.0, +3 VU -> 1.0
    auto const position = juce::jmap(meterAmplitude, minimumAmplitude, maximumAmplitude, 0.0f, 1.0f);
    auto const angle = juce::jmap(position, startAngle, endAngle);

    juce::Point<float> const pivot {
        getWidth() * 0.5f,
        getHeight() * 0.454f
    };
    auto const needleLength = getWidth() * 0.18f;
    juce::Point<float> const tip {
        pivot.x + std::cos(angle) * needleLength,
        pivot.y + std::sin(angle) * needleLength
    };

    // Offset shadow
    g.setColour(juce::Colours::black.withAlpha(0.25f));
    g.drawLine({ pivot.translated(1.0f, 1.0f), tip.translated(1.0f, 1.0f) },
               juce::jmax(1.5f, getWidth() * 0.0035f));
    // Needle
    g.setColour(juce::Colour::fromRGB(20, 18, 14));
    g.drawLine({ pivot, tip }, juce::jmax(1.2f, getWidth() * 0.0025f));
}

void NeuralAmpAudioProcessorEditor::timerCallback()
{
    constexpr auto zeroVuInDbfs = -18.0f; // 18 dB of headroom
    constexpr auto minimumDb = -20.0f;
    constexpr auto maximumDb = 3.0f;

    auto const outputDbfs = juce::Decibels::gainToDecibels(
        m_audioProcessor.getOutputMeterLevel(), -100.0f);
    m_meterDb = juce::jlimit(minimumDb, maximumDb, outputDbfs - zeroVuInDbfs);

    repaint(0, 0, getWidth(), juce::roundToInt(getHeight() * 0.48f));
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
