#include "widget.h"
#include "renderer.h"
#include "messenger.h"

#include <cassert>

#ifdef _DEBUG

const char* g_messageNames[] = {
	"LEFT_MOUSE_DOWN",
	"LEFT_MOUSE_UP",
	"RIGHT_MOUSE_DOWN",
	"RIGHT_MOUSE_UP",
	"MOVING_MOUSE",
	"MOUSE_LEFT",
	"MOUSE_ENTER",
	"KEY_PRESSED",
	"KEY_RELEASED",
	"WINDOW_RESIZED",
	"GOT_FOCUS",
	"LOST_FOCUS",
	"TIMER",
	"CUSTOM"
};

#endif 

/// <summary>
/// Pointer to the Widget that contains the cursor
/// </summary>
Widget* g_pCursorWidget = nullptr;

/// <summary>
/// Pointer to the Widget that has focus
/// </summary>
Widget* g_pFocusWidget = nullptr;

Widget::Widget(const Size& size, const Point& position, Widget* pParent) noexcept
{
	m_pParent = pParent;
	m_Size = size;
	setRelativePosition(position);
	setBackgroundColor(1.0f, 1.0f, 1.0f);
	
	if (pParent)
	{
		pParent->addChild(this);
	}
}

/// <summary>
/// 
/// </summary>
/// <returns>Returns the Widget size.</returns>
Size Widget::getSize(void) const noexcept
{
	return m_Size;
}

/// <summary>
/// </summary>
/// <returns>The width of the Widget.</returns>
uint32_t Widget::getWidth(void) const noexcept
{
	return m_Size.width;
}

/// <summary>
/// 
/// </summary>
/// <returns>The height of the Widget.</returns>
uint32_t Widget::getHeight(void) const noexcept
{
	return m_Size.height;
}

/// <summary>
/// Sets the Widget size.
/// </summary>
/// <returns></returns>
void Widget::setSize(const Size& size) noexcept
{
	m_Size = size;

	Message message = {};
	message.code = Message::Code::WINDOW_RESIZED;
	message.data = new Size(size);
	postMessage(message);
}

/// <summary>
/// Sets the Widget width.
/// </summary>
/// <param name="width"></param>
/// <returns></returns>
void Widget::setWidth(uint32_t width) noexcept
{
	m_Size.width = width;

	Message message = {};
	message.code = Message::Code::WINDOW_RESIZED;
	message.data = new Size(width, m_Size.height);
	postMessage(message);
}

/// <summary>
/// Sets the Widget height.
/// </summary>
/// <param name="height"></param>
/// <returns></returns>
void Widget::setHeight(uint32_t height) noexcept
{
	m_Size.height = height;

	Message message = {};
	message.code = Message::Code::WINDOW_RESIZED;
	message.data = new Size(m_Size.width, height);
	postMessage(message);
}

/// <summary>
/// 
/// </summary>
/// <param name=""></param>
/// <returns></returns>
Point Widget::getRelativePosition(void) const noexcept
{
	Point ptResult = {};
	ptResult.x = getRelativePositionX();
	ptResult.y = getRelativePositionY();

	return ptResult;
}

int32_t Widget::getRelativePositionX(void) const noexcept
{
	if (m_pParent)
	{
		return m_AbsolutePosition.x - m_pParent->getRelativePositionX();
	}

	return m_AbsolutePosition.x;
}

int32_t Widget::getRelativePositionY(void) const noexcept
{
	if (m_pParent)
	{
		return m_AbsolutePosition.y - m_pParent->getRelativePositionY();
	}

	return m_AbsolutePosition.y;
}

Point Widget::getAbsolutePosition(void) const noexcept
{
	return m_AbsolutePosition;
}

int32_t Widget::getAbsolutePositionX(void) const noexcept
{
	return m_AbsolutePosition.x;
}

int32_t Widget::getAbsolutePositionY(void) const noexcept
{
	return m_AbsolutePosition.y;
}

/// <summary>
/// Sets the position of the Widget relative to the top left of the parent Widget.
/// </summary>
/// <param name="pos"> Top left point position of the Widget relative to the parent </param>
/// <returns>Nothing</returns>
void Widget::setRelativePosition(const Point& pos) noexcept
{
	setRelativePositionX(pos.x);
	setRelativePositionY(pos.y);
}

/// <summary>
/// Sets the horizontal position of the Widget relative to the horizontal position of the parent Widget.
/// </summary>
/// <param name="x">Desired X coordinate of the Widget relative to the parent</param>
/// <returns></returns>
void Widget::setRelativePositionX(int32_t x) noexcept
{
	int32_t newAbsolutePosX = m_pParent ? x + m_pParent->getAbsolutePositionX() : x;

	for (Widget* pChild : m_Children) 
	{
		pChild->m_AbsolutePosition.x = (newAbsolutePosX + pChild->getRelativePositionX());
	}

	m_AbsolutePosition.x = newAbsolutePosX;
}

/// <summary>
/// Sets the vertical position of the Widget relative to the horizontal position of the parent Widget.
/// </summary>
/// <param name="y">Desired Y coordinate of the Widget relative to the parent</param>
/// <returns></returns>
void Widget::setRelativePositionY(int32_t y) noexcept
{
	int32_t newAbsolutePosY = m_pParent ? y + m_pParent->getAbsolutePositionY() : y;

	for (Widget* pChild : m_Children)
	{
		pChild->m_AbsolutePosition.y = newAbsolutePosY + pChild->getRelativePositionY();
	}

	m_AbsolutePosition.y = newAbsolutePosY;
}

/// <summary>
/// Sets the Widget as the focused Widget.
/// </summary>
void Widget::setFocus(void) noexcept
{
	g_pFocusWidget = this;

	Message message = {};
	message.code = Message::Code::GOT_FOCUS;
	message.data = nullptr;
	postMessage(message);
}

/// <summary>
///
/// </summary>
/// <returns>Whether the Widget has focus.</returns>
bool Widget::hasFocus(void) const noexcept
{
	return this == g_pFocusWidget;
}

/// <summary>
/// If the Widget has focus, then it loses focus and no Widget is focused as a result.
/// If the Widget doesn't have focus, nothing happens.
/// </summary>
/// <returns></returns>
void Widget::killFocus(void) noexcept
{
	if (hasFocus())
	{
		g_pFocusWidget = nullptr;

		Message message = {};
		message.code = Message::Code::LOST_FOCUS;
		message.data = nullptr;
		postMessage(message);
	}
}

/// <summary>
/// Retrieves the focused Widget.
/// </summary>
/// <returns></returns>
Widget* Widget::getFocusWidget(void) noexcept
{
	return g_pFocusWidget;
}

/// <summary>
/// Retrieves the Widget containing the cursor/
/// </summary>
/// <returns></returns>
Widget* Widget::getCursorWidget(void) noexcept
{
	return g_pCursorWidget;
}

/// <summary>
/// Returns the position of the cursor relative to the top left corner of the widget.
/// </summary>
/// <param name="out">Point structure that received the information</param>
void Widget::getRelativeCursorPosition(Point& out) const noexcept
{
	graphics::MouseState ms;
	graphics::getMouseState(ms);

	out.x = ms.cur_pos_x - getAbsolutePositionX();
	out.y = ms.cur_pos_y - getAbsolutePositionY();
}

/// <summary>
/// </summary>
/// <returns>A const reference to the list containing the Widget's children.</returns>
const std::list<Widget*>& Widget::getChildren(void) const noexcept
{
	return m_Children;
}

/// <summary>
/// Removes a child window, if it exists.
/// </summary>
/// <param name="pWidget"> Child window to be removed </param>
void Widget::removeChild(Widget* pWidget)
{
	assert(pWidget);

	std::list<Widget*>::iterator it;

	if ((it = std::find(m_Children.begin(), m_Children.end(), pWidget)) != m_Children.end())
	{
		// Since we are about to remove the widget, we want to update the cursor widget
		// variable in order to avoid a segmentation fault during the processing of
		// a mouse move message or something similar
		if (pWidget == g_pCursorWidget || pWidget->hasChild(g_pCursorWidget))
		{
			g_pCursorWidget = pWidget->m_pParent;
		}

		if (pWidget == g_pFocusWidget || pWidget->hasChild(g_pFocusWidget))
		{
			g_pFocusWidget = pWidget->m_pParent;
		}

		pWidget->cleanup();

		m_Children.erase(it);
	}
}

/// <summary>
/// Checks whether the window contains the given is a child of the widget, or a child of a child
/// </summary>
bool Widget::hasChild(Widget* pWidget)
{
	for (Widget* child : m_Children)
	{
		if (pWidget == child)
		{
			return true;
		}

		for (Widget* childOfChild : child->getChildren())
		{
			if (childOfChild->hasChild(pWidget))
			{
				return true;
			}
		}
	}

	return false;
}

/// <summary>
/// Removes the window from the parent.
/// </summary>
void Widget::removeFromParent(void)
{
	Widget* pParent = getParent();

	if (pParent)
	{
		pParent->removeChild(this);
	}
}

/// <summary>
/// </summary>
/// <returns>A pointer to the parent Widget</returns>
Widget* Widget::getParent(void) const noexcept
{
	return m_pParent;
}

/// <summary>
/// 
/// </summary>
/// <returns>The root Widget, meaning the Widget that is at the top of the Widget hierarchy.</returns>
Widget* Widget::getRoot(void) noexcept
{
	Widget* pParent = this->m_pParent;

	if (pParent == NULL) 
	{
		return this;
	}

	while (pParent->m_pParent)
	{
		pParent = pParent->m_pParent;
	}

	return pParent;
}

/// <summary>
/// Pops a message from the message queue (inbox) and forwards it to the appropriate Widget.
/// </summary>
void Widget::update(float ms)
{
	while (!m_Inbox.empty())
	{
		Message msg = m_Inbox.front();
		m_Inbox.pop();

		if (!Messenger::forwardMessage(this, msg))
		{
#ifdef _DEBUG
			printf("Widget: %x, Message: %s\n", this, g_messageNames[(int)msg.code]);
#endif
			processMessage(msg);
			MessageGenerator::deallocateMessage(msg);
		}
	}

	for (Widget* pChild : getChildren())
	{
		pChild->update(ms);
	}
}

void Widget::draw(void)
{
	drawBackground();
	drawChildren();
}

/// <summary>
/// Draws a rectangle using the background brush.
/// </summary>
void Widget::drawBackground(void) noexcept
{
	const uint32_t width = getWidth();
	const uint32_t height = getHeight();

	Renderer renderer(this);
	renderer.drawRect(
		width / 2.F,
		height / 2.F,
		static_cast<float>(width),
		static_cast<float>(height),
		m_bgBrush
	);
}

/// <summary>
/// Calls the draw function for each child Widget.
/// </summary>
/// <param name=""></param>
void Widget::drawChildren(void) noexcept
{
	for (Widget* pChildWidget : m_Children)
	{
		if (pChildWidget->m_opacity > 0.F)
		{
			pChildWidget->draw();
		}
	}
}

void Widget::bringToTop(void)
{
	if (m_pParent)
	{
		auto it = std::find(m_pParent->m_Children.begin(), m_pParent->m_Children.end(), this);

		Widget* pWidget = *it;

		m_pParent->m_Children.erase(it);
		m_pParent->m_Children.emplace_back(pWidget);
	}
}

/// <summary>
/// Sets the color that will be used to clear the background of the Widget.
/// </summary>
/// <param name="r"></param>
/// <param name="g"></param>
/// <param name="b"></param>
void Widget::setBackgroundColor(float r, float g, float b)
{
	m_bgBrush.fill_color[0] = r;
	m_bgBrush.fill_color[1] = g;
	m_bgBrush.fill_color[2] = b;
}

/// <summary>
/// Sets the color that will be used to color the outline of the background rectangle.
/// </summary>
/// <param name="r"></param>
/// <param name="g"></param>
/// <param name="b"></param>
void Widget::setOutlineColor(float r, float g, float b)
{
	m_bgBrush.outline_color[0] = r;
	m_bgBrush.outline_color[1] = g;
	m_bgBrush.outline_color[2] = b;
}

/// <summary>
/// 
/// </summary>
/// <param name="f"></param>
void Widget::setOpacity(float f)
{
	m_opacity = f;
	m_bgBrush.fill_opacity = f;
	m_bgBrush.outline_opacity = f;
}

void Widget::show(void)
{
	setOpacity(1.0F);
}

void Widget::hide(void)
{
	setOpacity(0.0F);
}

/// <summary>
/// Shows the widget if the argument given is true, otherwise hides it.
/// </summary>
/// <param name="b">Whether the widget should be shown</param>
void Widget::show(bool b)
{
	b ? show() : hide();
}

/// <summary>
/// Adds a child to the list of children.
/// </summary>
/// <param name="pWidget">Pointeer to the Widget that has this Widget as a parent.</param>
void Widget::addChild(Widget* pWidget)
{
	assert(pWidget);

	m_Children.emplace_back(pWidget);
}

void Widget::setParent(Widget* newParent) noexcept
{
	assert(newParent);

	if (m_pParent)
	{
		m_pParent->m_Children.remove(this);
	}

	newParent->addChild(this);
}

void Widget::addTimer(int timer_id, int delay)
{
	MessageGenerator::getInstance()->addTimer(this, timer_id, delay);
}

void Widget::killTimer(int timer_id)
{
	MessageGenerator::getInstance()->killTimer(this, timer_id);

	// We want to remove any timer messages that may be left in the queue after killing the timer
	std::queue<Message> newQueue;

	// We remove each message from the front of the inbox and add it to the new queue
	// if and only if the message isn't a timer message with the specified id
	while (!m_Inbox.empty())
	{
		Message frontMsg = m_Inbox.front();
		m_Inbox.pop();

		if (frontMsg.code == Message::Code::TIMER)
		{
			TimerMessageInfo* pInfo = reinterpret_cast<TimerMessageInfo*>(frontMsg.data);

			if (pInfo->timer_id == timer_id)
			{
				continue;
			}
		}

		newQueue.push(frontMsg);
	}

	// and finally we replace the inbox with the new queue
	m_Inbox.swap(newQueue);
}

/// <summary>
/// Adds a message to the message queue
/// </summary>
/// <param name="message">Reference to the message that will be posted</param>
void Widget::postMessage(const Message& message)
{
	m_Inbox.emplace(message);
}

/// <summary>
/// Calls the appropriate handler function for each message
/// </summary>
/// <param name="message">Reference to a message struct</param>
/// <returns>Depends on the message</returns>
long Widget::processMessage(const Message& message)
{
	switch (message.code)
	{
	case Message::Code::LEFT_MOUSE_DOWN:
		return onLeftMouseDown(*(Point*)message.data);

	case Message::Code::LEFT_MOUSE_UP:
		return onLeftMouseUp(*(Point*)message.data);

	case Message::Code::MOUSE_ENTER:
		return onMouseEnter((MouseMessageInfo*)message.data);

	case Message::Code::MOUSE_LEFT:
		return onMouseLeft((MouseMessageInfo*)message.data);

	case Message::Code::MOVING_MOUSE:
		return onDraggingMouse(*(Point*)message.data);

	case Message::Code::KEY_PRESSED:
		return onKeyPressed((KeyMessageInfo*)message.data);

	case Message::Code::KEY_RELEASED:
		return onKeyReleased((KeyMessageInfo*)message.data);

	case Message::Code::LOST_FOCUS:
		return onLostFocus();

	case Message::Code::GOT_FOCUS:
		return onGotFocus();

	case Message::Code::WINDOW_RESIZED: {
		Size* pSize = (Size*)message.data;
		return onResize(pSize->width, pSize->height);
	}

	case Message::Code::RIGHT_MOUSE_DOWN:
		return onRightMouseDown(*(Point*)message.data);

	case Message::Code::RIGHT_MOUSE_UP:
		return onRightMouseUp(*(Point*)message.data);

	case Message::Code::TIMER: {
		TimerMessageInfo* pTimerInfo = reinterpret_cast<TimerMessageInfo*>(message.data);
		return onTimer(pTimerInfo->timer_id);
	}

	case Message::Code::CUSTOM:
		return onCustom(reinterpret_cast<CustomMessageInfo*>(message.data));
	}

	return 1L;
}