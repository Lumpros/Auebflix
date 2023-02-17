#pragma once

#include "win/widget.h"

class TextEdit : public Widget
{
public:
	TextEdit(const Size& size, const Point& point, Widget* pParent);

	long onKeyPressed(KeyMessageInfo*) override;
	long onKeyReleased(KeyMessageInfo*) override;
	long onLeftMouseDown(const Point& point) override;
	long onGotFocus(void) override;
	long onLostFocus(void) override;
	long onTimer(int timer_id) override;

	void draw(void) override;
	void showText(bool show);
	void setPlaceholderText(const std::string& placeholder);

	std::string getText(void) const noexcept;

private:
	struct
	{
		bool m_isKeyDown = false;
		KeyMessageInfo keyInfo;
	} repeatedKeyInfo;

	bool m_isTextVisible = true;
	int m_iCaretIndex = 0;

	char m_buffer[128 + 1];
	int m_bufferIndex = 0;

	std::string placeholder = "";
	std::string displayedText = "";

private:
	void shiftBufferLeft(int startIndex);
	void shiftBufferRight(int startIndex);
	void shiftCharacter(char* c);
	void insertCharacter(KeyMessageInfo* pInfo);
	void updateDisplayedText(void);
};

