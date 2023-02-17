#pragma once

#include "message.h"
#include "types.h"

#include <vector>
#include <sgg/scancodes.h>

struct KeyMessageInfo
{
	graphics::scancode_t scancode;
	char key;

	bool isShiftPressed;
	bool isCtrlPressed;	
};

struct MouseMessageInfo
{
	bool isLeftPressed = false;
	bool isRightPressed = false;
	bool isMiddlePressed = false;
};

struct CustomMessageInfo
{
	int id;
	void* data;
};

class Widget;

struct TimerMessageInfo
{
	Widget* pRecipient = nullptr;
	int timer_id = 0;
};

/// <summary>
/// This class' responsibility is generating messages upon detecting mouse/keyboard events
/// </summary>
class MessageGenerator
{
public:
	static MessageGenerator* getInstance(void);

	MessageGenerator(const MessageGenerator&) = delete;
	MessageGenerator& operator=(const MessageGenerator&) = delete;

	std::vector<Message> generateMessages(float delta);
	static void deallocateMessage(const Message&);
	static MouseMessageInfo* generateMouseMessageInfo(void);

	void addTimer(Widget*, int timer_id, int delay);
	void killTimer(Widget*, int timer_id);

private:
	MessageGenerator(void);

	void generateMouseMessages(std::vector<Message>& messages);
	void generateKeyboardMessages(std::vector<Message>& messages);
	void generateTimerMessages(std::vector<Message>& messages, float delta);

	static bool isCtrlPressed(void);
	static bool isShiftPressed(void);
	static char scancodeToChar(graphics::scancode_t code);

private:
	/// <summary>
	/// The library is way too restrictive, so we have to keep track of the key state of every single key
	/// we care about, so we can send the appropriate message.
	/// 
	/// I should mention that the way of setting the array size and iterating it letter is not a good
	/// idea because the values could change in the future, however I really don't see any other way
	/// </summary>
	bool isKeyPressed[graphics::scancode_t::SCANCODE_KP_PERIOD - graphics::scancode_t::SCANCODE_A + 1];

	/// <summary>
	/// Holds the position of the cursor during the previous call to generateMouseMessages.
	/// We cannot use the pt_prev_x/y members of the mouse state structure, because then once
	/// the mouse is moved, the target window will receive infinite MOUSE_MOVE messages.
	/// </summary>
	Point ptPrevious;
};