#include "renderer.h"

/// <summary>
/// Draws a rectangle positioned relative to the parent of the target Widget.
/// </summary>
/// <param name="center_x">Horizontal center of the rectangle, relative to the parent Widget</param>
/// <param name="center_y">Vertical center of the rectangle, relative to the parent Widget</param>
/// <param name="width">Width of the rectangle</param>
/// <param name="height">Height of the rectangle</param>
/// <param name="brush">The brush that will be used to draw the rectangle</param>
void Renderer::drawRect(float center_x, float center_y, float width, float height, const graphics::Brush& brush)
{
	if (center_x + width < 0 || center_x - width > WINDOW_WIDTH || center_y + height < 0 || center_y - height > WINDOW_HEIGHT)
		return;

	graphics::drawRect(
		m_pTargetWidget->getAbsolutePositionX() + center_x,
		m_pTargetWidget->getAbsolutePositionY() + center_y,
		width,
		height,
		brush
	);
}

void Renderer::drawText(float pos_x, float pos_y, float size, const std::string& text, const graphics::Brush& brush)
{	
	const float drawPositionX = m_pTargetWidget->getAbsolutePositionX() + pos_x;
	const float drawPositionY = m_pTargetWidget->getAbsolutePositionY() + pos_y;

	if (drawPositionY >= 0 && drawPositionY - size <= WINDOW_HEIGHT)
	{
		graphics::drawText(
			drawPositionX,
			drawPositionY,
			size,
			text,
			brush
		);
	}
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, const graphics::Brush& brush)
{
	const int32_t absolute_x = m_pTargetWidget->getAbsolutePositionX();
	const int32_t absolute_y = m_pTargetWidget->getAbsolutePositionY();

	graphics::drawLine(
		x1 + absolute_x,
		y1 + absolute_y,
		x2 + absolute_x,
		y2 + absolute_y,
		brush
	);
}