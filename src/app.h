#pragma once

#include "win/widget.h"
#include "win/message_generator.h"

#include "FilmOrganizer.h"
#include "GenericScrollbar.h"
#include "SearchResultPanel.h"
#include "FilterControl.h"

#include <unordered_map>

class AppWindow : public Widget
{
public:
	AppWindow(Size size, std::string title);
	~AppWindow(void);

	long onCustom(CustomMessageInfo* info) override;

	void resize(int width, int height);
	void update(float ms);
	void draw();

	void doMessageLoop();

private:
	MessageGenerator* m_pMsgGenerator = nullptr;
	GenericScrollbar* m_pScrollbar = nullptr;
	SearchResultPanel* m_pSearchResultPanel = nullptr;
	FilterControl* m_pFilterControl = nullptr;

	/// <summary>
	/// We're going to use a hashmap to match the genre of the film organizer to the
	/// actual film organizer object. This way, when we want to add a film to an
	/// organizer, we can find the correct film organizer to add it to using the genre(s).
	/// </summary>
	std::unordered_map<std::string, FilmOrganizer*> m_filmOrganizers;

private:
	void closeFilmPanel(Widget* pPanel);
	void closeSearchResults(void);
	void showDrawnText(bool show);
	void showMainUI(bool show);
	void searchFilms(void);

	void initFilmOrganizers(void);
	void initScrollbar(void);
	void initSearchUI(void);
};