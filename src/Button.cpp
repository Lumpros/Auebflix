#include "Button.h"

long Button::onLeftMouseDown(const Point& point)
{
	m_isClicked = true;

	return 0L;
}

long Button::onLeftMouseUp(const Point& point)
{
	if (m_isClicked && m_isEnabled)
	{
		onClick();
	}

	m_isClicked = false;

	return 0L;
}

long Button::onMouseEnter(MouseMessageInfo* mmi)
{
	m_isHovered = true;

	return 0L;
}

long Button::onMouseLeft(MouseMessageInfo* mmi)
{
	m_isHovered = false;
	m_isClicked = false;

	return 0L;
}

void Button::enable(void)
{
	m_isEnabled = true;
}

void Button::disable(void)
{
	m_isEnabled = false;
}