#pragma once

#include <sgg/graphics.h>

#include <list>
#include <queue>
#include <cstdint>

#include "types.h"
#include "message.h"
#include "message_generator.h"

class Widget
{
public:
	Widget(const Size& size, const Point& position, Widget* pParent) noexcept;

	Size getSize(void) const noexcept;
	uint32_t getWidth(void) const noexcept;
	uint32_t getHeight(void) const noexcept;
	void setSize(const Size& size) noexcept;
	void setWidth(uint32_t width) noexcept;
	void setHeight(uint32_t height) noexcept;

	Point getRelativePosition(void) const noexcept;
	int32_t getRelativePositionX(void) const noexcept;
	int32_t getRelativePositionY(void) const noexcept;

	Point getAbsolutePosition(void) const noexcept;
	int32_t getAbsolutePositionX(void) const noexcept;
	int32_t getAbsolutePositionY(void) const noexcept;

	void setRelativePosition(const Point& pos) noexcept;
	void setRelativePositionX(int32_t x) noexcept;
	void setRelativePositionY(int32_t y) noexcept;

	void getRelativeCursorPosition(Point& out) const noexcept;

	void setBackgroundColor(float r, float g, float b);
	void setOutlineColor(float r, float g, float b);
	void setOpacity(float f);
	void show(void);
	void hide(void);
	void show(bool b);

	void setFocus(void) noexcept;
	bool hasFocus(void) const noexcept;
	void killFocus(void) noexcept;

	void addTimer(int timer_id, int delayMilliseconds);
	void killTimer(int timer_id);

	void bringToTop(void);

	static Widget* getFocusWidget(void) noexcept;
	static Widget* getCursorWidget(void) noexcept;

	const std::list<Widget*>& getChildren(void) const noexcept;
	void removeChild(Widget* pWidget);
	void removeFromParent(void);
	Widget* getParent(void) const noexcept;
	void setParent(Widget* newParent) noexcept;
	Widget* getRoot(void) noexcept;

	void postMessage(const Message&);

	virtual void update(float ms);
	virtual void draw(void);
	virtual void cleanup(void) {};

	virtual long onLeftMouseDown(const Point& point) { return 0L; }
	virtual long onLeftMouseUp(const Point& point) { return 0L; }
	virtual long onRightMouseDown(const Point& point) { return 0L; }
	virtual long onRightMouseUp(const Point& point) { return 0L; }
	virtual long onResize(uint32_t newWidth, uint32_t newHeight) { return 0L; }
	virtual long onKeyPressed(KeyMessageInfo* pInfo) { return 0L; }
	virtual long onKeyReleased(KeyMessageInfo* pInfo) { return 0L; }
	virtual long onGotFocus(void) { return 0L; }
	virtual long onLostFocus(void) { return 0L; }
	virtual long onMouseLeft(MouseMessageInfo*) { return 0L; }
	virtual long onMouseEnter(MouseMessageInfo*) { return 0L; }
	virtual long onDraggingMouse(Point point) { return 0L; }
	virtual long onTimer(int timer_id) { return 0L; }
	virtual long onCustom(CustomMessageInfo*) { return 0L; }

protected:
	void drawBackground(void) noexcept;
	void drawChildren(void) noexcept;

	bool hasChild(Widget* pWidget);

	float m_opacity = 1.0F;
	graphics::Brush m_bgBrush;

private:
	void addChild(Widget* pWidget);
	long processMessage(const Message& message);

private:
	Widget* m_pParent = nullptr;
	std::list<Widget*> m_Children;
	std::queue<Message> m_Inbox;

	Size m_Size;
	Point m_AbsolutePosition;
};