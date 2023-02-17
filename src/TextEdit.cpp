#include "TextEdit.h"
#include "win/renderer.h"

#define CARET_TOGGLE_TIME_MS 500

#define CARET_BLINK_TIMER 100
#define INITIAL_REPEAT_TIMER 101
#define FASTER_REPEAT_TIMER 102

#define DISPLAYED_CHARACTER_COUNT 30

#define STRING_SIZE(x) (sizeof(x) / sizeof((x)[0]) - 1)

TextEdit::TextEdit(const Size& size, const Point& point, Widget* pParent)
	: Widget(size, point, pParent)
{
	memset(m_buffer, 0, sizeof(m_buffer));
	memset(&repeatedKeyInfo.keyInfo, 0, sizeof(KeyMessageInfo));

	setOutlineColor(0.0F, 0.0F, 0.0F);
	setBackgroundColor(42 / 255.F, 42 / 255.F, 42 / 255.F);
}

long TextEdit::onKeyPressed(KeyMessageInfo* pInfo)
{
	if (!repeatedKeyInfo.m_isKeyDown)
	{
		repeatedKeyInfo.m_isKeyDown = true;
		repeatedKeyInfo.keyInfo = *pInfo;

		addTimer(INITIAL_REPEAT_TIMER, 400);
	}

	// The library can't print newline characters so we'll make this text edit only have 1 line because
	// I can't be bothered enough to implement multiple lines
	if (pInfo->scancode == graphics::scancode_t::SCANCODE_RETURN)
	{
		return 0L;
	}

	// We have to handle some special cases, such as backspace, left, right etc
	// because we can't directly print them to the screen (obviously)
	switch (pInfo->scancode)
	{
	case graphics::SCANCODE_BACKSPACE:
		if (m_iCaretIndex > 0)
		{
			m_iCaretIndex = std::max(0, m_iCaretIndex - 1);
			m_bufferIndex = std::max(0, m_bufferIndex - 1);
			shiftBufferLeft(m_iCaretIndex);
			updateDisplayedText();
		}
		return 0L;

	case graphics::scancode_t::SCANCODE_LEFT:
		m_iCaretIndex = std::max(0, m_iCaretIndex - 1);
		updateDisplayedText();
		return 0L;

	case graphics::scancode_t::SCANCODE_RIGHT:
		m_iCaretIndex = std::min(m_bufferIndex, m_iCaretIndex + 1);
		updateDisplayedText();
		return 0L;

	case graphics::scancode_t::SCANCODE_DOWN:
	case graphics::scancode_t::SCANCODE_UP:
		return 0L;
	}

	if (m_bufferIndex < STRING_SIZE(m_buffer))
	{
		insertCharacter(pInfo);
		updateDisplayedText();
	}

	return 0L;
}

void TextEdit::updateDisplayedText(void)
{
	displayedText = std::string(m_buffer).substr(std::max(m_iCaretIndex - DISPLAYED_CHARACTER_COUNT, 0), DISPLAYED_CHARACTER_COUNT);
}

long TextEdit::onKeyReleased(KeyMessageInfo*)
{
	repeatedKeyInfo.m_isKeyDown = false;

	killTimer(INITIAL_REPEAT_TIMER);
	killTimer(FASTER_REPEAT_TIMER);

	return 0L;
}

void TextEdit::insertCharacter(KeyMessageInfo* pInfo)
{
	if (m_iCaretIndex < m_bufferIndex)
	{
		shiftBufferRight(m_iCaretIndex);
	}

	// Characters are capitalized by default, so if shift isn't pressed
	// we convert them to lower. It is assumed that caps lock is not activated,
	// since we can't determine for sure whether it is or not using this library
	if (!pInfo->isShiftPressed)
	{
		pInfo->key = tolower(pInfo->key);
	}

	else
	{
		shiftCharacter(&pInfo->key);
	}

	m_buffer[m_iCaretIndex] = pInfo->key;

	++m_bufferIndex;
	++m_iCaretIndex;
}

/// <summary>
/// Shifts every character from the specified index up to the end to the left.
/// A null terminator is put in place of the previous last character
/// </summary>
/// <param name="startIndex">Index of the character that will be replaced</param>
void TextEdit::shiftBufferLeft(int startIndex)
{
	int i = startIndex;

	for (; i < (STRING_SIZE(m_buffer) - 1) && m_buffer[i]; ++i)
	{
		m_buffer[i] = m_buffer[i + 1];
	}

	m_buffer[i] = '\0';
}

/// <summary>
/// Shifts the buffer to the right by one
/// </summary>
/// <param name="startIndex"></param>
void TextEdit::shiftBufferRight(int startIndex)
{
	for (int i = m_bufferIndex; i > startIndex; --i)
	{
		m_buffer[i] = m_buffer[i - 1];
	}
}

/// <summary>
/// Converts a character to its equivelant character when SHIFT is pressed.
/// If the character doesn't have a matching shifted character, nothing happens.
/// </summary>
/// <param name="c">Pointer to the character that will be shifted</param>
void TextEdit::shiftCharacter(char* c)
{
	const char shifted[] = "!@#$%^&*(){}<>?:\"|+_~";
	const char received[] = "1234567890[],./;'\\=-`";

	for (int i = 0; i < STRING_SIZE(received); ++i)
	{
		if (received[i] == *c)
		{
			*c = shifted[i];
			return;
		}
	}
}

std::string TextEdit::getText(void) const noexcept
{
	return std::string(m_buffer);
}

long TextEdit::onLeftMouseDown(const Point& point)
{
	return 0L;
}

long TextEdit::onGotFocus(void)
{
	setOutlineColor(0.2F, 0.2F, 0.2F);

	return 0L;
}

long TextEdit::onLostFocus(void)
{
	setOutlineColor(.0F, .0F, .0F);

	return 0L;
}

long TextEdit::onTimer(int timer_id)
{
	switch (timer_id)
	{
	case INITIAL_REPEAT_TIMER:
		killTimer(INITIAL_REPEAT_TIMER);
		addTimer(FASTER_REPEAT_TIMER, 25);
		break;

	case FASTER_REPEAT_TIMER:
		return onKeyPressed(&repeatedKeyInfo.keyInfo);
	}

	return 0L;
}

/// <summary>
/// Toggles text drawing in order to avoid overlapping
/// </summary>
/// <param name="show"></param>
void TextEdit::showText(bool show)
{
	m_isTextVisible = show;
}

void TextEdit::draw(void)
{
	drawBackground();

	if (m_isTextVisible)
	{
		Renderer renderer(this);

		if (!m_buffer[0])
		{
			if (!placeholder.empty())
			{
				graphics::Brush brush;
				brush.fill_color[0] = 0.75F;
				brush.fill_color[1] = 0.75F;
				brush.fill_color[2] = 0.75F;

				renderer.drawText(10, 30, 20, placeholder, brush);
			}
		}

		else
		{
			graphics::Brush brush;
			brush.fill_color[0] = 1.0F;
			brush.fill_color[1] = 1.0F;
			brush.fill_color[2] = 1.0F;

			renderer.drawText(10, 30, 20, displayedText, brush);
		}
	}
}

void TextEdit::setPlaceholderText(const std::string& placeholder)
{
	this->placeholder = placeholder;
}