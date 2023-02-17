#pragma once

#include <sgg/graphics.h>

#include "widget.h"

#define WINDOW_WIDTH 1370
#define WINDOW_HEIGHT 720

class Renderer
{
public:
	Renderer(Widget* pWidget)
		: m_pTargetWidget{ pWidget } {}

	void drawRect(float center_x, float center_y, float width, float height, const graphics::Brush&);
	void drawText(float pos_x, float pos_y, float size, const std::string& text, const graphics::Brush&);
	void drawLine(float x1, float y1, float x2, float y2, const graphics::Brush&);

private:
	Widget* m_pTargetWidget;
};