#pragma once
#include <JuceHeader.h>

#define BORDER_WIDTH 1.5f
#define KNOB_SCALE 0.85f

class NeonTaxiwayLookAndFeel : public LookAndFeel_V4 {
public:
	NeonTaxiwayLookAndFeel();

	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider& slider) override;

	void setNumThicks(int newValue);

private:

	int numTicks = 10;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NeonTaxiwayLookAndFeel)
};
