#include "YearSlider.h"

YearSlider::YearSlider(const Size& size, const Point& point, Widget* pWidget)
	: Slider(size, point, pWidget), m_Renderer(this)
{
	m_thumbBrush.fill_color[0] = 0.4F;
	m_thumbBrush.fill_color[1] = 0.4F;
	m_thumbBrush.fill_color[2] = 0.4F;
	m_thumbBrush.fill_opacity = m_opacity;

	m_thumbBrush.outline_color[0] = 0.4F;
	m_thumbBrush.outline_color[1] = 0.4F;
	m_thumbBrush.outline_color[2] = 0.4F;
	m_thumbBrush.outline_opacity = m_opacity;

	m_lineBrush.fill_color[0] = 0.7F;
	m_lineBrush.fill_color[1] = 0.7F;
	m_lineBrush.fill_color[2] = 0.7F;
	m_lineBrush.fill_opacity = m_opacity;

	m_lineBrush.outline_color[0] = 0.7F;
	m_lineBrush.outline_color[1] = 0.7F;
	m_lineBrush.outline_color[2] = 0.7F;
	m_lineBrush.outline_opacity = m_opacity;

	m_thumbWidth = 14;
}

void YearSlider::draw(void)
{
	m_thumbBrush.fill_opacity = m_opacity;
	m_lineBrush.fill_opacity = m_opacity;

	if (m_showText)
	{
		m_Renderer.drawText(0.F, -5.F, 24.F, std::to_string(getValue()), m_thumbBrush);
	}

	m_Renderer.drawRect(getWidth() / 2.F, getHeight() / 2.F, (float)getWidth(), getHeight() / 10.F, m_lineBrush);
	m_Renderer.drawRect(m_thumbPosX + m_thumbWidth / 2.F, getHeight() / 2.F, (float)m_thumbWidth, (float)getHeight(), m_thumbBrush);
}

void YearSlider::showText(bool show)
{
	m_showText = show;
}