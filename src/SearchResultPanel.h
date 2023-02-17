#pragma once

#include "win/widget.h"

#include "Film.h"
#include "FilmButton.h"
#include "FilmInfoPanel.h"
#include "GenericScrollbar.h"

#include <list>

#define SHOW_MAIN_UI 300
#define CLOSE_SEARCH_RESULTS 301

class SearchResultCloseButton : public PanelCloseButton
{
public:
	SearchResultCloseButton(const Size& size, const Point& point, Widget* pParent)
		: PanelCloseButton(size, point, pParent) {}

private:
	void onClick(void) override;
};

class SearchResultPanel : public Widget
{
public:
	SearchResultPanel(const Size& size, const Point& point, Widget* pParent);
	~SearchResultPanel(void);

	void showText(bool show);

	long onTimer(int timer_id) override;
	long onCustom(CustomMessageInfo* pInfo) override;

	void draw(void) override;
	void setFilms(const std::list<Film*>& films);

private:
	void cleanupFilmButtons(void);

private:
	std::list<FilmButton*> m_filmButtons;

	SearchResultCloseButton* m_pCloseButton = nullptr;
	GenericScrollbar* m_pScrollbar = nullptr;

	std::string m_resultText;

	bool m_isTextVisible = true;
};