#include "NeonTaxiwayTheme.h"

NeonTaxiwayLookAndFeel::NeonTaxiwayLookAndFeel() {
}

void NeonTaxiwayLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider& slider) {
	
	const auto radius = jmin(width, height) * 0.5 * KNOB_SCALE - BORDER_WIDTH * 0.5;
	const auto centreX = x + width * 0.5;
	const auto centreY = y + height * 0.5;
	const auto kx = centreX - radius;
	const auto ky = centreY - radius;
	const auto kw = radius * 2.0;

	const Colour brightCol = Colour(0xff2b2b31);
	const Colour darkCol = Colour(0xff0d0d11);

	g.setGradientFill(ColourGradient(brightCol.brighter(0.05), kx, ky, brightCol, kx, ky + kw, false));
	g.fillEllipse(kx, ky, kw, kw);

	g.setGradientFill(ColourGradient(brightCol.brighter(0.9), kx, ky, darkCol.darker(0.05), kx, ky + kw, false));
	g.drawEllipse(kx, ky, kw, kw, BORDER_WIDTH);

	const auto pLen = radius * 0.333;
	const auto pThk = 2.0;

	Path p;
	p.addRectangle(pThk * -0.5, -radius, pThk, pLen);

	const auto angle = jmap(sliderPosProportional, rotaryStartAngle, rotaryEndAngle);
	p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));

	g.setGradientFill(ColourGradient(Colours::white, kx, ky, Colours::white.darker(0.2), kx, ky + kw, false));

	g.fillPath(p);

	Path originalShape, tempPath;
	const auto thikness = 2.0;
	originalShape.addEllipse(thikness * -0.5, radius / -KNOB_SCALE, thikness, thikness);
	g.setColour(Colours::grey);

	for (int i = 0; i < numTicks; ++i) {
		tempPath = originalShape;
		auto propPos = (float)i / (numTicks - 1);
		// propPos = pow(propPos, slider.getSkewFactor());
		auto angle = jmap(propPos, rotaryStartAngle, rotaryEndAngle);
		tempPath.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));
		g.fillPath(tempPath);
	}
}

void NeonTaxiwayLookAndFeel::setNumThicks(int newValue) {
	numTicks = newValue;
}
