#pragma once

#include "win/widget.h"

class Button : public Widget
{
public:
	Button(const Size& size, const Point& pos, Widget* pParent)
		: Widget(size, pos, pParent) {}

	long onLeftMouseDown(const Point& point) override;
	long onLeftMouseUp(const Point& point) override;
	virtual long onMouseEnter(MouseMessageInfo*) override;
	virtual long onMouseLeft(MouseMessageInfo*) override;

	void enable(void);
	void disable(void);
	inline bool isEnabled(void) const noexcept { return m_isEnabled; }

protected:
	virtual void onClick(void) = 0;

	bool m_isEnabled = true;
	bool m_isHovered = false;
	bool m_isClicked = false;
};

