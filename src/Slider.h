#pragma once

#include "win/widget.h"
#include "win/renderer.h"

class Slider : public Widget
{
public:
	Slider(const Size& size, const Point& point, Widget* pParent)
		: Widget(size, point, pParent), m_thumbHeight(size.height) {}

	long onLeftMouseDown(const Point& point) override;
	long onLeftMouseUp(const Point& point) override;
	long onMouseLeft(MouseMessageInfo* mmi) override;
	long onMouseEnter(MouseMessageInfo* mmi) override;
	long onDraggingMouse(Point point) override;

	void setRange(int min, int max) noexcept;
	void setValue(int value) noexcept;

	/// <summary>
	/// Returns the minimum value the slider can return.
	/// This value is returned when the thumb is at the left-most part of the slider.
	/// </summary>
	/// <returns></returns>
	inline int getMinValue(void) const noexcept 
	{
		return m_min; 
	}

	/// <summary>
	/// Returns the maximum value the slider can return.
	/// This value is returned when the thumb is at the right-msot part of the slider.
	/// </summary>
	/// <returns></returns>
	inline int getMaxValue(void) const noexcept 
	{
		return m_max; 
	}

	/// <summary>
	/// Returns the value that is currently selected by the slider.
	/// </summary>
	/// <returns></returns>
	inline int getValue(void) const noexcept 
	{ 
		return m_value; 
	}

protected:
	/// <summary>
	/// Used in the derived class to draw the widget.
	/// </summary>
	int m_thumbWidth = 20;
	int m_thumbHeight = 0;

	/// <summary>
	/// The X coordinate of the left part of the slider thumb.
	/// This ranges from 0 to this.width - thumb.width.
	/// </summary>
	int m_thumbPosX = 0;

private:
	int m_value = 0;

	/// <summary>
	/// We will 
	/// </summary>
	int m_min = 0, m_max = 100;

	bool m_isThumbBeingDragged = false;

private:
	int horizontalPositionToValue(int x);
	int valueToHorizontalPosition(int v);
};

