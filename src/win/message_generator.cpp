#include "message_generator.h"
#include "types.h"
#include "widget.h"

#include <sgg/graphics.h>
#include <list>

struct TimerInfo {
	int delay = 0;
	float time_passed = 0.F;
	int id = 0;
	Widget* pTarget = nullptr;
};

std::list<TimerInfo> g_TimerInfos;

#define STRING_SIZE(x) (sizeof(x) / sizeof((x)[0]))

MessageGenerator* MessageGenerator::getInstance()
{
	static MessageGenerator generator;
	return &generator;
}

MessageGenerator::MessageGenerator(void)
	: ptPrevious(0, 0)
{
	memset(isKeyPressed, false, sizeof(isKeyPressed));
}

std::vector<Message> MessageGenerator::generateMessages(float delta)
{
	std::vector<Message> messages;

	generateMouseMessages(messages);
	generateKeyboardMessages(messages);
	generateTimerMessages(messages, delta);

	return messages;
}

/// <summary>
/// Generates and appends LEFT_MOUSE_DOWN, LEFT_MOUSE_UP and a MOVING_MOUSE messages to the given vector.
/// </summary>
/// <param name="messages"></param>
void MessageGenerator::generateMouseMessages(std::vector<Message>& messages)
{
	graphics::MouseState ms;
	graphics::getMouseState(ms);

	Message msg = {};

	ms.cur_pos_x = static_cast<int>(graphics::windowToCanvasX((float)ms.cur_pos_x));
	ms.cur_pos_y = static_cast<int>(graphics::windowToCanvasY((float)ms.cur_pos_y));
	
	if (ms.button_left_pressed)
	{
		msg.code = Message::Code::LEFT_MOUSE_DOWN;
		msg.data = new Point(ms.cur_pos_x, ms.cur_pos_y);
		messages.emplace_back(msg);
	}

	else if (ms.button_left_released)
	{
		msg.code = Message::Code::LEFT_MOUSE_UP;
		msg.data = new Point(ms.cur_pos_x, ms.cur_pos_y);
		messages.emplace_back(msg);
	}

	// For some reason whenever ms.button_left_pressed is true, ms.button_right_pressed is true
	// EVEN THOUGH I'm not pressing the button, so we have to add an else here otherwise it won't
	// work. I don't know if this is a library bug or if it's just incompatible with Windows 11
	// but what I do know for certain is that it doesn't work properly...
	else if (ms.button_right_pressed)
	{
		msg.code = Message::Code::RIGHT_MOUSE_DOWN;
		msg.data = new Point(ms.cur_pos_x, ms.cur_pos_y);
		messages.emplace_back(msg);
	}

	else if (ms.button_right_released)
	{
		msg.code = Message::Code::RIGHT_MOUSE_UP;
		msg.data = new Point(ms.cur_pos_x, ms.cur_pos_y);
		messages.emplace_back(msg);
	}
	
	// If cur_pos != prev_pos then obviously the cursor has moved.
	if ((ms.cur_pos_x != ptPrevious.x) || (ms.cur_pos_y != ptPrevious.y))
	{
		msg.code = Message::Code::MOVING_MOUSE;
		msg.data = new Point(ms.cur_pos_x, ms.cur_pos_y);
		messages.emplace_back(msg);
	}

	ptPrevious = Point(ms.cur_pos_x, ms.cur_pos_y);
}

/// <summary>
/// Generates and appends KEY_PRESSED and KEY_RELEASED messages to the vector.
/// </summary>
/// <param name="messages"></param>
void MessageGenerator::generateKeyboardMessages(std::vector<Message>& messages)
{
	const bool shiftPressed = isShiftPressed();
	const bool ctrlPressed = isCtrlPressed();

	for (int i = 0; i < STRING_SIZE(isKeyPressed); ++i)
	{
		graphics::scancode_t code = (graphics::scancode_t)(i + (int)graphics::scancode_t::SCANCODE_A);
		bool isPressed = graphics::getKeyState(code);

		// Key state has changed (pressed/released)
		if (isPressed != isKeyPressed[i])
		{
			KeyMessageInfo* pInfo = new KeyMessageInfo();
			pInfo->scancode = code;
			pInfo->key = scancodeToChar(code);
			pInfo->isShiftPressed = shiftPressed;
			pInfo->isCtrlPressed = ctrlPressed;

			Message message = {};
			message.code = isPressed ? Message::Code::KEY_PRESSED : Message::Code::KEY_RELEASED;
			message.data = pInfo;
			messages.emplace_back(message);
		}

		isKeyPressed[i] = isPressed;
	}
}

/// <summary>
/// Generates TIMER messages
/// </summary>
/// <param name="messages"></param>
/// <param name="delta">Milliseconds passed since last update</param>
void MessageGenerator::generateTimerMessages(std::vector<Message>& messages, float delta)
{
	for (auto it = g_TimerInfos.begin(); it != g_TimerInfos.end(); ++it) 
	{
		it->time_passed += delta;

		if (static_cast<int>(it->time_passed >= it->delay))
		{
			it->time_passed = 0.F;
			
			TimerMessageInfo* messageInfo = new TimerMessageInfo;
			messageInfo->pRecipient = it->pTarget;
			messageInfo->timer_id = it->id;
			
			Message message = {};
			message.code = Message::Code::TIMER;
			message.data = messageInfo;
			messages.emplace_back(message);
		}
	}
}

/// <summary>
/// Schedules a TIMER message to be sent to the given widget with the specified id every N milliseconds.
/// </summary>
/// <param name="pWidget">Pointer to the widget which shall receive the TIMER messages</param>
/// <param name="timer_id">The id of the timer</param>
/// <param name="delay">Time between TIMER messages, in milliseconds</param>
void MessageGenerator::addTimer(Widget* pWidget, int timer_id, int delay)
{
	auto equalTimer = [&](const TimerInfo& info) {
		return info.id == timer_id && info.pTarget == pWidget;
	};

	if (std::find_if(std::begin(g_TimerInfos), std::end(g_TimerInfos), equalTimer) == g_TimerInfos.end())
	{
		TimerInfo info;
		info.delay = delay;
		info.id = timer_id;
		info.pTarget = pWidget;

		g_TimerInfos.emplace_back(info);
	}
}

/// <summary>
/// If the given timer exists for the given widget, it will stop receiving the specified timer message
/// </summary>
/// <param name="pWidget">Pointer to the widget that is receiving the timer message</param>
/// <param name="timer_id">Timer id specified during the call to addTimer</param>
void MessageGenerator::killTimer(Widget* pWidget, int timer_id)
{
	auto it = std::find_if(std::begin(g_TimerInfos), std::end(g_TimerInfos), [=](const TimerInfo& info) {
		return info.id == timer_id && info.pTarget == pWidget;
	});

	if (it != std::end(g_TimerInfos))
	{
		g_TimerInfos.erase(it);
	}
}

// This function must be called once the message has been processed, otherwise there might be a memory leak.
void MessageGenerator::deallocateMessage(const Message& msg)
{
	switch (msg.code)
	{
	case Message::Code::GOT_FOCUS:
	case Message::Code::LOST_FOCUS:
		break;
		
	default:
		if (msg.data)
			delete msg.data;
	}
}

/// <summary>
/// Allocates a MouseMessageInfo struct and fills it with information.
/// MUST be deleted by the caller.
/// This function exists because some mouse messages are generated outside of this class,
/// for example during the processing of a message in the messenger class, so we want to
/// give MessageGenerator the responsibility of initializing the parameters of the new message.
/// </summary>
/// <param name="mmi">Reference to the mouse message info struct to be filled</param>
MouseMessageInfo* MessageGenerator::generateMouseMessageInfo(void)
{
	graphics::MouseState ms;
	graphics::getMouseState(ms);

	MouseMessageInfo* mmi = new MouseMessageInfo;
	mmi->isLeftPressed = ms.button_left_down;
	mmi->isMiddlePressed = ms.button_middle_down;
	mmi->isRightPressed = ms.button_right_down;

	return mmi;
}

char MessageGenerator::scancodeToChar(graphics::scancode_t code)
{
	const char match[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890\n\x1B\b\t -=[]\\#;'`,./                           /*-+\n1234567890.";

	return match[code - graphics::scancode_t::SCANCODE_A];
}

bool MessageGenerator::isCtrlPressed(void)
{
	return graphics::getKeyState(graphics::scancode_t::SCANCODE_LCTRL) || graphics::getKeyState(graphics::scancode_t::SCANCODE_RCTRL);
}

bool MessageGenerator::isShiftPressed(void)
{
	return graphics::getKeyState(graphics::scancode_t::SCANCODE_LSHIFT) || graphics::getKeyState(graphics::scancode_t::SCANCODE_RSHIFT);
}