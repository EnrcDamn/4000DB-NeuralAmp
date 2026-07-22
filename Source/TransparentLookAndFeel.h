#pragma once
#include <JuceHeader.h>

/**
 * A custom LookAndFeel class that makes knob sliders transparent and draws
 * a circular indicator dot.
 */

class TransparentLookAndFeel : public juce::LookAndFeel_V4
{
public:
    TransparentLookAndFeel();
    ~TransparentLookAndFeel() {};

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransparentLookAndFeel)
};