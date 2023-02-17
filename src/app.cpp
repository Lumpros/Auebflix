#include "app.h"

#include <algorithm>

#include "TextEdit.h"
#include "FilmInfoPanel.h"
#include "YearSlider.h"
#include "SearchButton.h"
#include "GenreButton.h"
#include "SearchResultPanel.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

AppWindow::AppWindow(Size size, std::string title)
	: Widget(size, Point(0, 0), nullptr)
{
	m_pMsgGenerator = MessageGenerator::getInstance();

	graphics::createWindow(size.width, size.height, title);
	graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);
	graphics::setCanvasSize((float)size.width, (float)size.height);

	graphics::setDrawFunction(std::bind(&AppWindow::draw, this));
	graphics::setUpdateFunction(std::bind(&AppWindow::update, this, std::placeholders::_1));
	graphics::setResizeFunction(std::bind(&AppWindow::resize, this, std::placeholders::_1, std::placeholders::_2));

	initFilmOrganizers();
	initScrollbar();
	initSearchUI();

	setBackgroundColor(0.0F, 0.0F, 0.0F);
	setOutlineColor(0.0F, 0.0F, 0.0F);

	graphics::setFont("assets\\SEGOEUI.ttf");
}

AppWindow::~AppWindow(void)
{
	for (const std::pair<std::string, FilmOrganizer*>& orgPair : m_filmOrganizers)
	{
		delete orgPair.second;
	}

	delete m_pScrollbar;
	delete m_pFilterControl;
	
	if (m_pSearchResultPanel)
	{
		delete m_pSearchResultPanel;
	}
}

/// <summary>
/// Creates the FilmOrganizer objects, aligns them on the screen and adds movies to each one appropriately
/// </summary>
void AppWindow::initFilmOrganizers(void)
{
	const char* genres[] = { "Adventure", "Fantasy", "SciFi", "Comedy", "Action", "Drama"};

	constexpr int organizerWidth = 1340;
	constexpr int organizerHeight = 360;

	// We'll generate an organizer for each one of the genres listed above.
	// We'll also add it to the hashmap to use later.
	for (int i = ARRAY_SIZE(genres) - 1; i >= 0; --i)
	{
		FilmOrganizer* pOrganizer = new FilmOrganizer(Size(organizerWidth, organizerHeight), Point(0, 500 + i * organizerHeight), this);
		pOrganizer->setBackgroundColor(0.0F, 0.0F, 0.0F);
		pOrganizer->setLabel(genres[i]);
		m_filmOrganizers[genres[i]] = pOrganizer;
	}

	// Here we add each film to the FilmOrganizers whose category is included in the film's category set
	// For example, if a film's categories are "Fantasy" and "Adventure" then it is added to the
	// FilmOrganizers that have the label "Fantasy" and "Adventure" (2).
	for (Film* pFilm : Film::getLoadedFilms())
	{
		for (const std::string& genre : pFilm->getGenres())
		{
			m_filmOrganizers[genre]->addFilm(pFilm);
		}
	}
}

/// <summary>
/// 
/// </summary>
/// <param name=""></param>
void AppWindow::initScrollbar(void)
{
	m_pScrollbar = new GenericScrollbar(static_cast<int>(m_filmOrganizers.size()) * 340, this);
}

/// <summary>
/// Initialize the filter control
/// </summary>
/// <param name=""></param>
void AppWindow::initSearchUI(void)
{
	m_pFilterControl = new FilterControl(Point(950, 120), this);
}

/// <summary>
/// 
/// </summary>
/// <param name="ms"></param>
void AppWindow::update(float ms)
{
	for (Message& message : m_pMsgGenerator->generateMessages(ms))
	{
		this->postMessage(message);
	}

	Widget::update(ms);
}

long AppWindow::onCustom(CustomMessageInfo* pInfo)
{
	switch (pInfo->id)
	{
	case CLOSE_FILM_INFO_PANEL_MESSAGE:
		closeFilmPanel(reinterpret_cast<Widget*>(pInfo->data));
		break;

	case CLOSE_SEARCH_RESULTS:
		closeSearchResults();
		break;

	case HIDE_DRAWN_TEXT:
		showDrawnText(false);
		break;

	case SEARCH_FILMS_BUTTON_PRESSED:
		searchFilms();
		break;

	case SHOW_MAIN_UI:
		showMainUI(pInfo->data);
		break;

	case SHOW_RESULT_TEXT:
		if (m_pSearchResultPanel)
			m_pSearchResultPanel->showText(static_cast<bool>(pInfo->data));
		break;
	}

	return 0L;
}

/// <summary>
/// Closes the search result panel and releases any allocated resources
/// </summary>
void AppWindow::closeSearchResults(void)
{
	m_pSearchResultPanel->removeFromParent();

	delete m_pSearchResultPanel;

	// May be deleted again in the destructor so we need to set
	// this pointer to null in order to avoid deleting a dangling pointer.
	m_pSearchResultPanel = nullptr;

	showDrawnText(true);
}

void AppWindow::searchFilms(void)
{
	std::list<Film*> films = m_pFilterControl->searchFilms();
	
	// We don't need to check if m_pSearchResultPanel is pointing to anything, because
	// it is assumed that the search result panel has either not been initialized at all,
	// or it has been previously been initialized and then deleted during the 
	// processing of the CLOSE_SEARCH_RESULTS custom message.
	m_pSearchResultPanel = new SearchResultPanel(Size(getWidth(), getHeight()), Point(0, 0), this);
	m_pSearchResultPanel->setFilms(films);
}

/// <summary>
/// Shows/Hides the film organizers, the filter UI, and generally anything that is shown in the first screen.
/// This is done for two reasons:
///		1. For some reason text is drawn on top of EVERYTHING, so we have to hide it in order to avoid widgets overlapping
///		2. The program is obviously much faster if we don't draw widgets that aren't visible
/// </summary>
/// <param name="show">True if should show, false if should hide</param>
void AppWindow::showMainUI(bool show)
{
	m_pFilterControl->show(show);

	for (const std::pair<std::string, FilmOrganizer*>& orgPair : m_filmOrganizers)
	{
		orgPair.second->show(show);
	}
}

/// <summary>
/// Removes the film info panel from the window's children and shows/hides the appropriate children
/// </summary>
void AppWindow::closeFilmPanel(Widget* pPanel)
{
	pPanel->removeFromParent();

	// We allocated the panel inside the film button class, so we must do some cleanup here
	delete pPanel;

	// If the search result panel isn't null, this means that the FilmInfoPanel
	// was created from the search results, meaning that the drawn text must not
	// become visible yet, because the search results are on top.
	// If the search result panel is visible, then the drawn text will be shown
	// when it is closed and closeSearchResults() is called.
	if (!m_pSearchResultPanel)
	{
		showDrawnText(true);
	}
}

/// <summary>
/// Shows/Hides all the text that is being drawn in the main ui in order to avoid
/// drawing text on top of the film info panel and the search results.
/// </summary>
/// <param name="show"></param>
void AppWindow::showDrawnText(bool show)
{
	for (auto& pair : m_filmOrganizers)
	{
		pair.second->showText(show);
	}

	m_pFilterControl->showText(show);
}


void AppWindow::draw(void)
{
	drawBackground();

	const int distanceScrolled = m_pScrollbar->getScrollDistance();
	const float backgroundWidth = static_cast<float>(getWidth()) - m_pScrollbar->getWidth();
	
	graphics::Brush brush;
	brush.outline_opacity = 0.F;
	brush.texture = "assets\\main-bg.png";

	// We'll scroll the background image by one fifth of the total distance scrolled
	// This makes the ui feel more interactive and 3d and idk it looks pretty good
	graphics::drawRect(backgroundWidth / 2.F, 300.F - distanceScrolled / 5.F, backgroundWidth, backgroundWidth * 9.F / 16.F, brush);


	brush.texture = "";
	brush.fill_color[0] = 0.F;
	brush.fill_color[1] = 0.F;
	brush.fill_color[2] = 0.F;

	// Here we draw the black rectangle that's at the top of the screen
	graphics::drawRect(backgroundWidth / 2.F, 50.F - distanceScrolled, backgroundWidth, 100.F, brush);


	brush.texture = "assets\\auebflix-logo.png";
	brush.fill_color[0] = 1.0F;
	brush.fill_color[1] = 1.0F;
	brush.fill_color[2] = 1.0F;

	// and finally we draw the Auebflix logo inside the rectangle
	graphics::drawRect(backgroundWidth / 2.F, 50.F - distanceScrolled, 349.F, 100.F, brush);


	brush.texture = "assets\\moto.png";

	graphics::drawRect(400, 300.F - distanceScrolled, 469, 149, brush);

	drawChildren();
}

void AppWindow::resize(int width, int height)
{
	//Widget::setSize(Size((uint32_t)width, (uint32_t)height));
}

void AppWindow::doMessageLoop()
{
	graphics::startMessageLoop();
}