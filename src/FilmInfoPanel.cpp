#include "FilmInfoPanel.h"
#include "FilmButton.h"

#define FADE_IN_TIMER 100

FilmInfoPanel::FilmInfoPanel(Film* film, Widget* pRoot)
	: Widget(pRoot->getSize(), Point(0, 0), pRoot), m_Renderer(this)
{
	m_pFilm = film;

	// We initialize a timer here which will make the window fade in view and then stop
	// Not really that important but makes the UI feel more interactive idk
	addTimer(FADE_IN_TIMER, 10);

	// So obviously we want the initial opacity to be 0, otherwise there would be
	// no point in fading the widget in
	m_opacity = 0.0F;

	// If the genres given are: "Adventure", "Fantasy", "SciFi"
	// we want to produce the string "Adventure,    Fantasy,     SciFi"
	// which we will draw later. We do this in the constructor because doing
	// this every time the window is drawn would be wasteful.
	for (const std::string& genre : m_pFilm->getGenres())
	{
		// Also should mention that I left a lot of spaces because it's almost impossible
		// to notice a single space with this library...
		genre_string += genre + ",    ";
	}

	// Remove the last ',' and end the string there.
	genre_string.resize(genre_string.find_last_of(','));

	// When the PanelCloseButton is pressed, a message is sent to the AppWindow
	// During the processing of said message, removeFromParent is called for FilmInfoPanel
	// Inside removeFromParent in widget.h the cleanup method is called, which deletes this pointer
	m_pCloseButton = new PanelCloseButton(Size(48, 48), Point(10, 10), this);
	m_pCloseButton->bringToTop();
	m_pCloseButton->setOpacity(0.0F);

	initBrushes();
}

void FilmInfoPanel::initBrushes(void)
{
	m_bgImageBrush.texture = "assets\\main-bg.png";
	m_bgImageBrush.outline_opacity = 0.0F;
	m_bgImageBrush.fill_opacity = m_opacity;
	m_bgImageBrush.fill_color[0] = 0.4F;
	m_bgImageBrush.fill_color[1] = 0.4F;
	m_bgImageBrush.fill_color[2] = 0.4F;

	m_thumbnailBrush.texture = m_pFilm->getThumbnail();
	m_thumbnailBrush.outline_opacity = 0.0F;

	m_descBrush.fill_color[0] = 0.8F;
	m_descBrush.fill_color[1] = 0.8F;
	m_descBrush.fill_color[2] = 0.8F;
}

long FilmInfoPanel::onTimer(int timer_id)
{
	m_opacity += 0.125F;
	m_pCloseButton->setOpacity(m_opacity);

	if (m_opacity >= 1.F) 
	{
		killTimer(FADE_IN_TIMER);

		// This is 99.9% not necessary since we increase m_opacity by a power of 2
		// but I don't have the energy to think right now so I'll do this to be certain everything is ok
		m_opacity = 1.F;
		m_pCloseButton->setOpacity(1.F);
	}
	
	return 0L;
}

void FilmInfoPanel::cleanup()
{
	delete m_pCloseButton;
}

void FilmInfoPanel::draw(void)
{
	drawBackgroundImage();
	drawTitle();
	drawGenres();
	drawYear();
	drawDescription();
	drawThumbnail();
	drawDirector();
	drawStars();

	drawChildren();
}

void FilmInfoPanel::drawBackgroundImage(void)
{
	const float width = static_cast<float>(getWidth());
	const float height = static_cast<float>(getHeight());

	m_bgImageBrush.fill_opacity = m_opacity;

	m_Renderer.drawRect(
		width / 2.F,
		height / 2.F,
		width,
		height,
		m_bgImageBrush
	);
}

void FilmInfoPanel::drawTitle(void)
{
	graphics::Brush brush;
	brush.fill_opacity = m_opacity;

	m_Renderer.drawText(510.F, getHeight() / 2.F - 320 + 90, 40.F, m_pFilm->getName(), brush);
}

void FilmInfoPanel::drawGenres(void)
{
	graphics::Brush brush;
	brush.fill_opacity = m_opacity;

	m_Renderer.drawText(510.F, getHeight() / 2.F - 320 + 120, 20.F, std::to_string(m_pFilm->getYear()), brush);
}

void FilmInfoPanel::drawYear(void)
{
	graphics::Brush brush;
	brush.fill_opacity = m_opacity;

	m_Renderer.drawText(575.F, getHeight() / 2.F - 320 + 120, 20.F, genre_string, brush);
}

void FilmInfoPanel::drawDescription(void)
{
	const float yOffset = getHeight() / 2.F - 300 + 150;

	const std::vector<std::string>& description = m_pFilm->getDescription();
	const size_t lineCount = description.size();

	m_descBrush.fill_opacity = m_opacity;

	for (int i = 0; i < lineCount; ++i)
	{
		m_Renderer.drawText(510.F, yOffset + i * 30, 25.F, description[i], m_descBrush);
	}
}

void FilmInfoPanel::drawThumbnail(void)
{
	m_thumbnailBrush.fill_opacity = m_opacity;

	m_Renderer.drawRect(300.F, getHeight() / 2.F, 350.F, 525.F, m_thumbnailBrush);
}

void FilmInfoPanel::drawDirector(void)
{
	graphics::Brush brush;
	brush.fill_opacity = m_opacity;

	m_Renderer.drawText(510.F, 450, 30.F, m_pFilm->getDirector(), brush);
}

void FilmInfoPanel::drawStars(void)
{
	graphics::Brush brush;
	brush.fill_opacity = m_opacity;

	int counter = 0;

	for (const std::string& star : m_pFilm->getStars())
	{
		m_Renderer.drawText(510.F, 520.F + counter++ * 35.F, 30.F, star, brush);
	}
}

/// <summary>
/// Sends a message to the root window (AppWindow) that implies that the FilmInfoPanel widget should be closed
/// </summary>
/// <param name=""></param>
void PanelCloseButton::onClick(void)
{
	CustomMessageInfo* pMessageInfo = new CustomMessageInfo();
	pMessageInfo->id = CLOSE_FILM_INFO_PANEL_MESSAGE;
	// A pointer to the panel info widget is needed by AppWindow in order to close it when the message is received
	pMessageInfo->data = getParent(); 

	Message message = {};
	message.code = Message::Code::CUSTOM;
	message.data = pMessageInfo;
	getRoot()->postMessage(message);


	pMessageInfo = new CustomMessageInfo;
	pMessageInfo->id = SHOW_RESULT_TEXT;
	pMessageInfo->data = (void*)true;

	message.code = Message::Code::CUSTOM;
	message.data = pMessageInfo;
	getParent()->getParent()->postMessage(message);
}

void PanelCloseButton::draw(void)
{
	graphics::Brush brush;
	brush.texture = "assets\\x.png";
	brush.outline_opacity = 0.F;
	brush.fill_opacity = m_opacity;

	if (m_isHovered)
	{
		brush.fill_color[0] = 0.8F;
		brush.fill_color[1] = 0.8F;
		brush.fill_color[2] = 0.8F;

		if (m_isClicked)
		{
			brush.fill_color[0] = 0.6F;
			brush.fill_color[1] = 0.6F;
			brush.fill_color[2] = 0.6F;
		}
	}

	Renderer renderer(this);
	renderer.drawRect(getWidth() / 2.F, getHeight() / 2.F, (float)getWidth(), (float)getHeight(), brush);
}