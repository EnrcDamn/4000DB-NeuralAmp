#include "TransparentLookAndFeel.h"

TransparentLookAndFeel::TransparentLookAndFeel()
{
    setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
}

void TransparentLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPosProportional, float rotaryStartAngle,
                                        float rotaryEndAngle, juce::Slider& slider)
{
    // Background stays transparent, don't fill anything

    // Calculate needle angle from current value
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    auto centreX = x + width  * 0.5f;
    auto centreY = y + height * 0.5f;
    const auto radius = juce::jmin(width, height) * 0.385f;
    const auto indicatorRadius = radius * 0.8f; // We want it to be "inside" the knob

    // Draw a white indicator dot at the end of the rotary path.
    juce::Point<float> tip (
        centreX + indicatorRadius * std::sin(angle),
        centreY - indicatorRadius * std::cos(angle)
    );

    // Fade from grey at either endpoint to white at the middle (top) position.
    const auto midpointAmount = 1.0f - std::abs(2.0f * sliderPosProportional - 1.0f);
    g.setColour(juce::Colours::lightgrey.interpolatedWith(juce::Colours::white, midpointAmount));

    // Scale it depending on the knob size, with 6 px minimum. Size of the dot is 5% of the knob size.
    const auto dotDiameter = juce::jmax(8.0f, juce::jmin(width, height) * 0.05f);

    g.fillEllipse(
        tip.x - dotDiameter * 0.5f,
        tip.y - dotDiameter * 0.5f,
        dotDiameter,
        dotDiameter
    );

    // Outline circle around tip
    g.setColour(juce::Colours::darkgrey);
    g.drawEllipse(
        tip.x - dotDiameter * 0.5f,
        tip.y - dotDiameter * 0.5f,
        dotDiameter,
        dotDiameter,
        2.0f
    );
}
