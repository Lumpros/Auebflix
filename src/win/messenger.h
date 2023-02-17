#pragma once

#include "message.h"
#include "types.h"

class Widget;

/// <summary>
/// This class' responsibility is receiving a message and forwarding it to the Widget that it is meant for.
/// </summary>
class Messenger
{
public:
	// Returns true if the message was forwarded
	// Returns false if it wasn't => The message was meant for the receiver
	static bool forwardMessage(Widget* pReceiver, const Message& message);

private:
	static bool isPointInWidget(Widget* pWidget, Point point);
	static void absoluteToRelative(Point& point, Widget* pWidget);
	
	static Widget* findChildWidgetUnderPoint(Widget* pParent, const Point&);

	static bool forwardLeftMouseDown(Widget* pReceiver, const Message& message);
	static bool forwardLeftMouseUp(Widget* pReceiver, const Message& message);
	static bool forwardResize(Widget* pReceiver, const Message& message);
	static bool forwardKeyPressed(Widget* pReceiver, const Message& message);
	static bool forwardKeyReleased(Widget* pReceiver, const Message& message);
	static bool forwardGotFocus(Widget* pReceiver, const Message& message);
	static bool forwardDraggingMouse(Widget* pReceiver, const Message& message);
	static bool forwardTimer(Widget* pReceiver, const Message& message);
};