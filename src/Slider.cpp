#include "Slider.h"

#include <cassert>

/// <summary>
/// Sets the range of integers the slider's value can be included in.
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <returns></returns>
void Slider::setRange(int min, int max) noexcept
{
	assert(min != max);

	if (min > max)
	{
		std::swap(min, max);
	}

	m_min = min;
	m_max = max;

	m_value = horizontalPositionToValue(m_thumbPosX);
}

void Slider::setValue(int value) noexcept
{
	if (m_min <= value && value <= m_max)
	{
		m_thumbPosX = valueToHorizontalPosition(value);
		m_value = value;
	}
}

/// <summary>
/// Moves the center of the thumb to the cursor, but doesn't let the thumb's
/// edges move outside of the slider's bounds.
/// </summary>
/// <param name="point">Relative position of the cursor</param>
/// <returns>Return code</returns>
long Slider::onLeftMouseDown(const Point& point)
{
	m_isThumbBeingDragged = true;
	m_thumbPosX = std::max(0, std::min(point.x - m_thumbWidth / 2, static_cast<int>(getWidth()) - m_thumbWidth));
	m_value = horizontalPositionToValue(m_thumbPosX);

	return 0L;
}

/// <summary>
/// Does the same thing as onLeftMouseDown (literally Ctrl+C -> Ctrl+V)
/// </summary>
/// <param name="point">Relative position of the cursor</param>
/// <returns>Return code</returns>
long Slider::onDraggingMouse(Point point)
{
	if (m_isThumbBeingDragged)
	{
		m_thumbPosX = std::max(0, std::min(point.x - m_thumbWidth / 2, static_cast<int>(getWidth()) - m_thumbWidth));
		m_value = horizontalPositionToValue(m_thumbPosX);
	}

	return 0L;
}

/// <summary>
/// Calculates which value in the given range matches the thumb's horizontal position the most. 
/// </summary>
/// <param name="x">Relative horizontal position</param>
/// <returns>Matching value</returns>
int Slider::horizontalPositionToValue(int x)
{
	const double horizontalPositionRatio = ((double)x / ((int)getWidth() - m_thumbWidth));

	// Basically a linear function where f(0) = m_min and f(max) = m_max 
	return static_cast<int>(round(m_min + (horizontalPositionRatio * (m_max - m_min))));
}

/// <summary>
/// Inverse of horizontalPositionToValue
/// </summary>
/// <param name="v"></param>
/// <returns></returns>
int Slider::valueToHorizontalPosition(int v)
{
	return static_cast<int>(round((v - m_min) * (((double)getWidth() - m_thumbWidth) / (m_max - m_min))));
}

long Slider::onLeftMouseUp(const Point& point)
{
	m_isThumbBeingDragged = false;

	return 0L;
}

long Slider::onMouseLeft(MouseMessageInfo* mmi)
{
	m_isThumbBeingDragged = false;

	return 0L;
}

/// <summary>
/// It can be annoying when you're dragging the thumb and the cursor leaves the window on accident
/// and you have to reclick the thumb in order for the dragging to continue. Therefore if the mouse
/// re-enters the slider after dragging it, it will move to the appropriate position
/// </summary>
/// <param name="mmi"></param>
/// <returns></returns>
long Slider::onMouseEnter(MouseMessageInfo* mmi)
{
	if (!mmi->isLeftPressed)
	{
		m_isThumbBeingDragged = false;
	}

	else
	{
		Point simulatedClickPoint = {};
		// Ideally the MouseMessageInfo would contain the point of entry, however
		// I'm not in the mood to implement that right now so this will do.
		getRelativeCursorPosition(simulatedClickPoint);

		return onLeftMouseDown(simulatedClickPoint);
	}

	return 0L;
}