#pragma once

#include "win/widget.h"
#include "win/renderer.h"

class Scrollbar : public Widget
{
public:
	Scrollbar(int scrollDistance, Widget* pParent);

	long onLeftMouseDown(const Point& point) override;
	long onLeftMouseUp(const Point& point) override;
	long onDraggingMouse(Point point) override;
	long onMouseLeft(MouseMessageInfo* mmi) override;
	long onMouseEnter(MouseMessageInfo* mmi) override;
	long onTimer(int timer_id) override;

	void draw(void) override;
	int getScrollDistance(void) const noexcept;

protected:
	virtual void scroll(int dist) = 0;

private:
	int getBarY(void) const noexcept;
	int toScrollUnits(int y);

	void scrollUp();
	void scrollDown();

private:
	bool m_isTopBtnHovered = false;
	bool m_isBotBtnHovered = false;
	bool m_isDraggingBar = false;
	bool m_isHoveringBar = false;

	// This variable represents the distance between the cursor and the top
	// of the scrollbar when it is clicked. It is used later, during processing
	// of the dragging mouse message, in order to determine where the scrollbar
	// should be placed.
	int m_CursorOffsetFromBarTop = 0;

	// This variable represents the maximum distance the parent window's content
	// can be scrolled
	int m_maxScroll = 0;

	// This is the distance that the parent window has been scrolled
	int m_currentScroll = 0;

	// The scrollbar height
	int m_barHeight;
	
	// The distance the parent window is scrolled by upon pressing the top/bottom buttons
	int m_jump;

	int m_prevCursorY = 0;

	Renderer m_Renderer;
};

