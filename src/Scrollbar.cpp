#include "Scrollbar.h"

#include <cassert>

#define SCROLL_BUTTON_WIDTH 30
#define SCROLL_BUTTON_HEIGHT SCROLL_BUTTON_WIDTH

#define INITIAL_SCROLL_UP_TIMER 200
#define SCROLL_UP_TIMER 201
#define INITIAL_SCROLL_DOWN_TIMER 202
#define SCROLL_DOWN_TIMER 203

Scrollbar::Scrollbar(int scrollDistance, Widget* pParent)
	: Widget(Size(SCROLL_BUTTON_WIDTH, pParent->getHeight()), Point(pParent->getWidth() - SCROLL_BUTTON_WIDTH, 0), pParent)
	, m_Renderer(this)
{
	assert(scrollDistance > 0);

	m_maxScroll = scrollDistance;
	m_barHeight = std::min((int)getHeight() - SCROLL_BUTTON_HEIGHT * 2, std::max(150000 / scrollDistance, 100));
	// This value is completely arbitrary, it works well so we use it lol
	m_jump = std::min(25, scrollDistance);

	setBackgroundColor(42 / 255.F, 42 / 255.F, 42 / 255.F);
	setOutlineColor(42 / 255.F, 42 / 255.F, 42 / 255.F);
}

/// <summary>
/// If the top button is clicked, then we scroll up a bit
/// If the bottom button was clicked, then we scroll down a bit
/// If the bar was clicked, then we initiate the dragging process
/// If none of the above, we move the center of the bar to the cursor
/// </summary>
/// <param name="point">Point clicked relative to the window</param>
/// <returns>Return code</returns>
long Scrollbar::onLeftMouseDown(const Point& point)
{
	if (point.y <= SCROLL_BUTTON_HEIGHT)
	{
		addTimer(INITIAL_SCROLL_UP_TIMER, 300);
		scrollUp();
	}

	else if (point.y >= (int)getHeight() - SCROLL_BUTTON_HEIGHT)
	{
		addTimer(INITIAL_SCROLL_DOWN_TIMER, 300);
		scrollDown();
	}

	else
	{
		killTimer(INITIAL_SCROLL_UP_TIMER);
		killTimer(INITIAL_SCROLL_DOWN_TIMER);
		killTimer(SCROLL_UP_TIMER);
		killTimer(SCROLL_DOWN_TIMER);

		int barY = getBarY();
		const int prevScroll = m_currentScroll;
		m_prevCursorY = point.y;

		// If the point clicked isn't inside the bar, we move the
		// scrollbar to the cursor before we start the dragging process.
		if (point.y < barY || point.y > barY + m_barHeight)
		{
			m_currentScroll = toScrollUnits(point.y - m_barHeight / 2);
			// We recalculate the scrollbar Y coordinate after changing the m_currentScroll
			// variable because we will need this later to calculate some stuff
			barY = getBarY();
			m_isHoveringBar = true;
			scroll(m_currentScroll - prevScroll);
		}

		m_CursorOffsetFromBarTop = point.y - barY;
		m_isDraggingBar = true;
	}

	return 0L;
}

void Scrollbar::scrollUp(void)
{
	int prevScroll = m_currentScroll;
	m_currentScroll = std::max(m_currentScroll - m_jump, 0);
	scroll(m_currentScroll - prevScroll);

	if (m_currentScroll == 0)
	{
		killTimer(INITIAL_SCROLL_UP_TIMER);
		killTimer(SCROLL_UP_TIMER);
	}
}

void Scrollbar::scrollDown(void)
{
	int before = m_currentScroll;
	m_currentScroll = std::min(m_currentScroll + m_jump, m_maxScroll);
	scroll(m_currentScroll - before);

	if (m_currentScroll == m_maxScroll)
	{
		killTimer(INITIAL_SCROLL_DOWN_TIMER);
		killTimer(SCROLL_DOWN_TIMER);
	}
}

long Scrollbar::onTimer(int timer_id)
{
	switch (timer_id)
	{
	case INITIAL_SCROLL_UP_TIMER:
		killTimer(INITIAL_SCROLL_UP_TIMER);
		addTimer(SCROLL_UP_TIMER, 50);
		break;

	case INITIAL_SCROLL_DOWN_TIMER:
		killTimer(INITIAL_SCROLL_DOWN_TIMER);
		addTimer(SCROLL_DOWN_TIMER, 50);
		break;

	case SCROLL_UP_TIMER:
		if (!m_isTopBtnHovered)
			killTimer(SCROLL_UP_TIMER);
		else
			scrollUp();
		break;

	case SCROLL_DOWN_TIMER:
		if (!m_isBotBtnHovered)
			killTimer(SCROLL_DOWN_TIMER);
		else
			scrollDown();
		break;
	}

	return 0L;
}

/// <summary>
/// Converts a point in the scroll space to a scroll unit, meaning a value that
/// m_currentScroll can store to represent the percentage of distance scrolled.
/// </summary>
/// <param name="y">Point in the scroll space</param>
int Scrollbar::toScrollUnits(int y)
{
	const int maxScrollbarY = getHeight() - SCROLL_BUTTON_HEIGHT - m_barHeight;

	return (int)((std::min(std::max(y, 0), maxScrollbarY) / static_cast<float>(maxScrollbarY)) * m_maxScroll);
}

/// <summary>
/// Calculates the Y coordinate of the scrollbar.
/// 
/// If m_currentScroll has reached the maximum value, then we want the scrollbar Y
/// to be at the maximum point, which is height - SB_HEIGHT - barHeight.
/// 
/// If m_currentScroll is 0, then we want the scrollbar Y to be at the minimum point,
/// which is SB_HEIGHT.
/// </summary>
int Scrollbar::getBarY(void) const noexcept
{
	return (int)((m_currentScroll / (float)m_maxScroll) * (getHeight() - 2 * SCROLL_BUTTON_HEIGHT - m_barHeight) + SCROLL_BUTTON_HEIGHT);
}

/// <summary>
/// Moves the scrollbar appropriately, if it is being dragged.
/// </summary>
/// <param name="point"></param>
/// <returns></returns>
long Scrollbar::onDraggingMouse(Point point)
{
	const int barY = getBarY();

	m_isHoveringBar = (point.y >= barY && point.y <= barY + m_barHeight);
	m_isTopBtnHovered = (point.y <= SCROLL_BUTTON_HEIGHT);
	m_isBotBtnHovered = (point.y >= (int)getHeight() - SCROLL_BUTTON_HEIGHT);

	if (m_isDraggingBar)
	{
		const int prevScroll = m_currentScroll;
		m_currentScroll = toScrollUnits(point.y - m_CursorOffsetFromBarTop);
		scroll(m_currentScroll - prevScroll);
	}

	m_prevCursorY = point.y;

	return 0L;
}

/// <summary>
/// Lets the object know that the scrollbar is no longer being dragged
/// </summary>
/// <param name="point">Relative point of the cursor when the left mouse was released</param>
/// <returns>Return code</returns>
long Scrollbar::onLeftMouseUp(const Point& point)
{
	killTimer(INITIAL_SCROLL_UP_TIMER);
	killTimer(INITIAL_SCROLL_DOWN_TIMER);
	killTimer(SCROLL_UP_TIMER);
	killTimer(SCROLL_DOWN_TIMER);

	m_isDraggingBar = false;
	
	return 0L;
}

/// <summary>
/// Lets the object know that the scrollbar is not being hovered or dragged anymore.
/// </summary>
/// <param name="mmi">Pointer to a MouseMessageInfo struct</param>
/// <returns>Return code</returns>
long Scrollbar::onMouseLeft(MouseMessageInfo* mmi)
{
	killTimer(INITIAL_SCROLL_UP_TIMER);
	killTimer(INITIAL_SCROLL_DOWN_TIMER);
	killTimer(SCROLL_UP_TIMER);
	killTimer(SCROLL_DOWN_TIMER);

	m_isHoveringBar = false;
	m_isDraggingBar = false;
	m_isBotBtnHovered = false;
	m_isTopBtnHovered = false;

	return 0L;
}

long Scrollbar::onMouseEnter(MouseMessageInfo* mmi)
{
	if (mmi->isLeftPressed)
	{
		Point simulatedClickPoint = {};
		getRelativeCursorPosition(simulatedClickPoint);
		return onLeftMouseDown(simulatedClickPoint);
	}

	return 0L;
}

/// <summary>
/// Returns the distance theat the scrollbar has moved the sibling widgets.
/// </summary>
/// <param name=""></param>
int Scrollbar::getScrollDistance(void) const noexcept
{
	return m_currentScroll;
}

/// <summary>
/// Draws the top scroll button, the bottom scroll button and the scrollbar.
/// The colors may change depending on what is being clicked/hovered.
/// </summary>
/// <param name=""></param>
void Scrollbar::draw(void)
{
	drawBackground();

	graphics::Brush brush;
	brush.outline_opacity = 0;

	if (!m_isHoveringBar) 
	{
		brush.fill_color[0] = 100 / 255.F;
		brush.fill_color[1] = 100 / 255.F;
		brush.fill_color[2] = 100 / 255.F;
	}

	else
	{
		brush.fill_color[0] = 120 / 255.F;
		brush.fill_color[1] = 120 / 255.F;
		brush.fill_color[2] = 120 / 255.F;
	}

	m_Renderer.drawRect(SCROLL_BUTTON_WIDTH / 2.F, getBarY() + m_barHeight / 2.F, SCROLL_BUTTON_WIDTH * 3.F / 5.F, (float)m_barHeight, brush);


	if (m_isTopBtnHovered)
	{
		brush.fill_color[0] = 120 / 255.F;
		brush.fill_color[1] = 120 / 255.F;
		brush.fill_color[2] = 120 / 255.F;

		m_Renderer.drawRect(SCROLL_BUTTON_WIDTH / 2.F, SCROLL_BUTTON_HEIGHT / 2.F, SCROLL_BUTTON_WIDTH, SCROLL_BUTTON_HEIGHT, brush);
	}

	else
	{
		brush.fill_color[0] = 1.0F;
		brush.fill_color[1] = 1.0F;
		brush.fill_color[2] = 1.0F;
	}

	brush.texture = "assets\\up-arrow.png";
	

	m_Renderer.drawRect(SCROLL_BUTTON_WIDTH / 2.F, SCROLL_BUTTON_HEIGHT / 2.F, SCROLL_BUTTON_WIDTH, SCROLL_BUTTON_HEIGHT, brush);


	if (m_isBotBtnHovered)
	{
		brush.texture = "";
		brush.fill_color[0] = 120 / 255.F;
		brush.fill_color[1] = 120 / 255.F;
		brush.fill_color[2] = 120 / 255.F;

		m_Renderer.drawRect(SCROLL_BUTTON_WIDTH / 2.F, getHeight() - SCROLL_BUTTON_HEIGHT / 2.F, (float)SCROLL_BUTTON_WIDTH, (float)SCROLL_BUTTON_HEIGHT, brush);
	}
	
	else
	{
		brush.fill_color[0] = 1.0F;
		brush.fill_color[1] = 1.0F;
		brush.fill_color[2] = 1.0F;
	}

	brush.texture = "assets\\down-arrow.png";

	m_Renderer.drawRect(SCROLL_BUTTON_WIDTH / 2.F, getHeight() - SCROLL_BUTTON_HEIGHT / 2.F, (float)SCROLL_BUTTON_WIDTH, (float)SCROLL_BUTTON_HEIGHT, brush);
}