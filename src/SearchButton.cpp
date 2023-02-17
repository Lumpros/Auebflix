#include "SearchButton.h"

#include "win/renderer.h"

SearchButton::SearchButton(const Size& size, const Point& point, Widget* pWidget)
	: Button(size, point, pWidget)
{
	setOutlineColor(0.0F, 0.F, 0.F);
}

void SearchButton::showText(bool show)
{
	m_isTextVisible = show;
}

void SearchButton::draw(void)
{
	if (m_isHovered)
	{
		if (m_isClicked)
		{
			setBackgroundColor(0.2F, 0.2F, 0.2F);
		}

		else
		{
			setBackgroundColor(0.3F, 0.3F, 0.3F);
		}
	}

	else
	{
		setBackgroundColor(0.1F, 0.1F, 0.1F);
	}

	drawBackground();

	if (m_isTextVisible)
	{
		graphics::Brush brush;

		Renderer renderer(this);
		renderer.drawText(120, 35, 25, "Search", brush);
	}
}

void SearchButton::onClick(void)
{
	CustomMessageInfo* pInfo = new CustomMessageInfo;
	pInfo->data = nullptr;
	pInfo->id = SEARCH_FILMS_BUTTON_PRESSED;

	Message message = {};
	message.code = Message::Code::CUSTOM;
	message.data = pInfo;

	getRoot()->postMessage(message);
}