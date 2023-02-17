#include "SearchResultPanel.h"

// For the FILM_WIDTH/FILM_HEIGHT definitions
#include "FilmOrganizer.h"

#define FADE_IN_TIMER 100

/// <summary>
/// Sends a CLOSE_SEARCH_RESULTS and a SHOW_MAIN_UI custom message to the root widget.
/// </summary>
/// <param name=""></param>
void SearchResultCloseButton::onClick(void)
{
	CustomMessageInfo* pInfo = new CustomMessageInfo;
	pInfo->id = CLOSE_SEARCH_RESULTS;
	pInfo->data = nullptr;

	Message message = {};
	message.code = Message::Code::CUSTOM;
	message.data = pInfo;
	getRoot()->postMessage(message);


	pInfo = new CustomMessageInfo;
	pInfo->id = SHOW_MAIN_UI;
	pInfo->data = (void*)true;

	message.code = Message::Code::CUSTOM;
	message.data = pInfo;
	getRoot()->postMessage(message);
}

SearchResultPanel::SearchResultPanel(const Size& size, const Point& point, Widget* pParent)
	: Widget(size, point, pParent)
{
	setOutlineColor(0.F, 0.F, 0.F);
	setBackgroundColor(0.F, 0.F, 0.F);

	hide();
	addTimer(FADE_IN_TIMER, 15);

	m_pCloseButton = new SearchResultCloseButton(Size(48, 48), Point(10, 10), this);

	// We'll send a message to the main window telling it to hide all the
	// text that is currently being drawn in order to avoid having the text drawn
	// on top of the search result panel
	CustomMessageInfo* pInfo = new CustomMessageInfo();
	pInfo->id = HIDE_DRAWN_TEXT;
	pInfo->data = (void*)true;

	Message message = {};
	message.code = Message::Code::CUSTOM;
	message.data = pInfo;
	getRoot()->postMessage(message);
}

SearchResultPanel::~SearchResultPanel(void)
{
	cleanupFilmButtons();

	delete m_pCloseButton;
	delete m_pScrollbar;
}

void SearchResultPanel::cleanupFilmButtons(void)
{
	for (FilmButton* pFilmButton : m_filmButtons)
	{
		delete pFilmButton;
	}

	m_filmButtons.clear();
}

/// <summary>
/// Increases the opacity until the window is fully visible
/// </summary>
/// <param name="timer_id">ID of the timer</param>
/// <returns>Return code</returns>
long SearchResultPanel::onTimer(int timer_id)
{
	setOpacity(m_opacity + 0.25F);

	for (FilmButton* pButton : m_filmButtons)
	{
		pButton->setOpacity(m_opacity + 0.25F);
	}

	if (m_opacity >= 1.F)
	{
		setOpacity(1.0F);
		killTimer(FADE_IN_TIMER);

		CustomMessageInfo* pInfo = new CustomMessageInfo;
		pInfo->id = SHOW_MAIN_UI;
		pInfo->data = (void*)false;

		Message message = {};
		message.code = Message::Code::CUSTOM;
		message.data = pInfo;
		getRoot()->postMessage(message);
	}

	return 0L;
}

/// <summary>
/// 
/// </summary>
/// <param name="films"></param>
void SearchResultPanel::setFilms(const std::list<Film*>& films)
{
	constexpr int filmsPerRow = 5;
	const size_t filmCount = films.size();

	cleanupFilmButtons();

	m_resultText = std::to_string(filmCount) + " result" + (filmCount != 1 ? "s found" : " found");;
	
	int i = 0;

	for (Film* pFilm : films)
	{
		// This is deleted in the destructor, or during a call to cleanupFilmButtons (in case 
		// setFilms is called twice for the same object, although it shouldn't)
		FilmButton* pFilmButton = new FilmButton(
			Size(FILM_WIDTH, FILM_HEIGHT),
			Point(150 + (i % filmsPerRow) * (FILM_WIDTH + 20), 100 + (i / filmsPerRow) * (FILM_HEIGHT + 20)),
			this
		);

		pFilmButton->setFilm(pFilm);
		pFilmButton->setOutlineColor(0.F, 0.F, 0.F);
		pFilmButton->hide();

		m_filmButtons.emplace_back(pFilmButton);

		++i;
	}

	// We'll only create a scrollbar if there are at least 2 rows of films.
	// For that reason, after we delete the old scrollbar, we must set the pointer to null,
	// because there is a chance we won't create a new scrollbar, in which case if we call
	// this function again we'll delete a dangling pointer.

	if (m_pScrollbar)
	{
		delete m_pScrollbar;
		m_pScrollbar = nullptr;
	}

	if (filmCount > filmsPerRow)
	{
		m_pScrollbar = new GenericScrollbar(((int)films.size() / filmsPerRow) * 300, this);
	}
}

void SearchResultPanel::showText(bool show)
{
	m_isTextVisible = show;
}

void SearchResultPanel::draw(void)
{
	drawBackground();

	if (m_isTextVisible)
	{
		int scrollDist = (m_pScrollbar ? m_pScrollbar->getScrollDistance() : 0);

		Renderer renderer(this);
		graphics::Brush brush;
		renderer.drawText(550.F, 60.F - scrollDist, 40.F, m_resultText, brush);
	}

	drawChildren();
}

long SearchResultPanel::onCustom(CustomMessageInfo* pInfo)
{
	if (pInfo->id == SHOW_RESULT_TEXT)
	{
		showText(static_cast<bool>(pInfo->data));
	}

	return 0L;
}