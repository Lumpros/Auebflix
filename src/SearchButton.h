#pragma once

#include "Button.h"

#define SEARCH_FILMS_BUTTON_PRESSED 400

class SearchButton : public Button
{
public:
	SearchButton(const Size& size, const Point& point, Widget* pWidget);

	void draw(void) override;
	void showText(bool show);

protected:
	void onClick(void) override;

private:
	bool m_isTextVisible = true;
};

