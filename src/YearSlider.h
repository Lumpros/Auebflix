#pragma once

#include "Slider.h"
#include "win/renderer.h"

class YearSlider : public Slider
{
public:
	YearSlider(const Size& size, const Point& point, Widget* pWidget);

	void draw(void) override;

	void showText(bool show);

private:
	Renderer m_Renderer;

	graphics::Brush m_lineBrush;
	graphics::Brush m_thumbBrush;

	bool m_showText = true;
};

