#include "FilmOrganizer.h"

#define FILM_MARGIN 20

#define SCROLL_RIGHT_TIMER 300
#define SCROLL_RIGHT_MESSAGE 300
#define SCROLL_LEFT_TIMER 301
#define SCROLL_LEFT_MESSAGE 301

FilmOrganizer::FilmOrganizer(const Size& size, const Point& point, Widget* pWidget)
	: Widget(size, point, pWidget) 
{
	setOutlineColor(0, 0, 0);

	m_pLeftButton = new FilmOrganizerScrollButton(Size(50, size.height - 40), Point(0, 40), this);
	m_pLeftButton->setDirection(FOSBDirection::LEFT);

	m_pRightButton = new FilmOrganizerScrollButton(Size(50, size.height - 40), Point(getWidth() - 50, 40), this);
	m_pRightButton->setDirection(FOSBDirection::RIGHT);

	updateScrollButtons();

	m_pLeftButton->hide();
	m_pRightButton->hide();
}

FilmOrganizer::~FilmOrganizer(void)
{
	for (auto it = m_FilmButtons.begin(); it != m_FilmButtons.end(); ++it)
	{
		delete (*it);
	}

	delete m_pLeftButton;
	delete m_pRightButton;
}

long FilmOrganizer::onCustom(CustomMessageInfo* info)
{
	switch (info->id)
	{
	case SCROLL_LEFT_MESSAGE:
		addTimer(SCROLL_LEFT_TIMER, 10);
		killTimer(SCROLL_RIGHT_TIMER);
		// Call this once here to avoid waiting the first time
		onTimer(SCROLL_LEFT_TIMER);			
		break;

	case SCROLL_RIGHT_MESSAGE:
		addTimer(SCROLL_RIGHT_TIMER, 10);
		killTimer(SCROLL_LEFT_TIMER);
		// Call this once here to avoid waiting the first time
		onTimer(SCROLL_RIGHT_TIMER);
		break;
	}

	return 0L;
}

long FilmOrganizer::onTimer(int timer_id)
{
	int scroll = (timer_id == SCROLL_LEFT_TIMER) ? 20 : (-20);

	// We will keep track of the total distance we have scrolled so the timer knows when to stop istelf.
	m_distanceScrolled += scroll;

	// We do the actual scrolling here
	for (FilmButton* pButton : m_FilmButtons) 
	{
		pButton->setRelativePositionX(
			pButton->getRelativePositionX() + scroll
		);
	}

	// We want to scroll the films by FILM_WIDTH + FILM_MARGIN so that they're aligned
	// properly, so if the distance is divisible by the said distance, we may stop
	if (m_distanceScrolled % (FILM_WIDTH + FILM_MARGIN) == 0)
	{
		killTimer(SCROLL_LEFT_TIMER);
		killTimer(SCROLL_RIGHT_TIMER);
	}

	// The buttons may need to be disabled and hidden if we've reached the end of the list
	// so we call this function to check if we've reached the end, and if we have, it alters the buttons' state
	updateScrollButtons();

	return 0L;
}

void FilmOrganizer::updateScrollButtons(void)
{
	if (m_FilmButtons.empty()) {
		m_pLeftButton->hide();
		m_pRightButton->hide();
		return;
	}

	if (m_FilmButtons.front()->getRelativePositionX() > 0) {
		m_pLeftButton->disable();
		m_pLeftButton->hide();
	} else {
		m_pLeftButton->enable();
		m_pLeftButton->show();
	}

	if (m_FilmButtons.back()->getRelativePositionX() + FILM_WIDTH < (int32_t)getWidth()) {
		m_pRightButton->disable();
		m_pRightButton->hide();
	} else {
		m_pRightButton->enable();
		m_pRightButton->show();
	}
}

/// <summary>
/// Creates a FilmButton for the given film and adds it to the end of the 'list' in the UI
/// </summary>
/// <param name="film">Film object containing the required information</param>
void FilmOrganizer::addFilm(Film* film)
{
	FilmButton* pFilmButton = new FilmButton(
		Size(FILM_WIDTH, FILM_HEIGHT),
		Point((int)m_FilmButtons.size() * (FILM_MARGIN + FILM_WIDTH) + FILM_MARGIN, 60),
		this
	);
	
	m_pLeftButton->bringToTop();
	m_pRightButton->bringToTop();

	pFilmButton->setFilm(film);
	m_FilmButtons.emplace_back(pFilmButton);

	updateScrollButtons();
	m_pLeftButton->hide();
	m_pRightButton->hide();
}

void FilmOrganizer::setLabel(const std::string& str)
{
	m_genreLabel = str;
}

long FilmOrganizer::onMouseEnter(MouseMessageInfo* mmi)
{
	if (!m_FilmButtons.empty())
	{
		if (m_pLeftButton->isEnabled())
			m_pLeftButton->show();

		if (m_pRightButton->isEnabled())
			m_pRightButton->show();
	}

	return 0L;
}

long FilmOrganizer::onMouseLeft(MouseMessageInfo* mmi)
{
	if (!m_FilmButtons.empty() && !hasChild(Widget::getCursorWidget()))
	{
		if (m_pLeftButton->isEnabled())
			m_pLeftButton->hide();

		if (m_pRightButton->isEnabled())
			m_pRightButton->hide();
	}

	return 0L;
}

void FilmOrganizer::draw(void)
{
	drawBackground();

	if (m_showText)
	{
		Renderer renderer(this);

		graphics::Brush brush;
		brush.fill_color[0] = 1.F;
		brush.fill_color[1] = 1.F;
		brush.fill_color[2] = 1.F;

		renderer.drawText(FILM_MARGIN, 40, 30, m_genreLabel, brush);
	}

	drawChildren();
}

void FilmOrganizer::showText(bool show)
{
	m_showText = show;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FilmOrganizerScrollButton::FilmOrganizerScrollButton(const Size& size, const Point& point, Widget* pWidget)
	: Button(size, point, pWidget)
{
	m_bgBrush.fill_color[0] = 0.0F;
	m_bgBrush.fill_color[1] = 0.0F;
	m_bgBrush.fill_color[2] = 0.0F;
	m_bgBrush.fill_secondary_color[0] = 0.0F;
	m_bgBrush.fill_secondary_color[1] = 0.0F;
	m_bgBrush.fill_secondary_color[2] = 0.0F;
	m_bgBrush.gradient = true;
	m_bgBrush.gradient_dir_u = 1.F;
	m_bgBrush.gradient_dir_v = 0.F;
	m_bgBrush.outline_opacity = 0.0F;
}

void FilmOrganizerScrollButton::setDirection(FOSBDirection dir)
{
	m_direction = dir;
}

void FilmOrganizerScrollButton::onClick(void)
{
	CustomMessageInfo* pInfo = new CustomMessageInfo;
	pInfo->id = (m_direction == FOSBDirection::LEFT) ? SCROLL_LEFT_MESSAGE : SCROLL_RIGHT_MESSAGE;
	pInfo->data = NULL;

	Message message = {};
	message.code = Message::Code::CUSTOM;
	message.data = pInfo;
	getParent()->postMessage(message);
}

long FilmOrganizerScrollButton::onMouseEnter(MouseMessageInfo* mmi)
{
	getParent()->onMouseEnter(mmi);

	return 0L;
}

long FilmOrganizerScrollButton::onMouseLeft(MouseMessageInfo* mmi)
{
	getParent()->onMouseLeft(mmi);

	return 0L;
}

void FilmOrganizerScrollButton::draw(void)
{
	if (m_opacity == 0.0F)
		return;

	Renderer renderer(this);

	graphics::Brush arrowBrush;
	arrowBrush.outline_opacity = 0.0F;

	if (m_direction == FOSBDirection::LEFT)
	{	
		arrowBrush.texture = "assets\\left-arrow.png";
		m_bgBrush.fill_secondary_opacity = 0.0F;
	}

	else 
	{
		arrowBrush.texture = "assets\\right-arrow.png";
		m_bgBrush.fill_opacity = 0.0F;
	}

	
	m_bgBrush.outline_opacity = 0.0F;
	renderer.drawRect(getWidth() / 2.F, getHeight() / 2.F, (float)getWidth(), (float)getHeight(), m_bgBrush);

	renderer.drawRect(getWidth() / 2.F, getHeight() / 2.F, 16.F, 32.F, arrowBrush);
}