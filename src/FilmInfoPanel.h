#pragma once

#include "win/widget.h"
#include "win/renderer.h"
#include "Film.h"
#include "Button.h"

#define CLOSE_FILM_INFO_PANEL_MESSAGE 200

class PanelCloseButton : public Button
{
public:
	PanelCloseButton(const Size& size, const Point& point, Widget* pParent)
		: Button(size, point, pParent) {}
		
	void draw(void) override;

protected:
	void onClick(void) override;
};

class FilmInfoPanel : public Widget
{
public:
	FilmInfoPanel(Film* pFilm, Widget* pRoot);

	void draw(void) override;
	void cleanup(void) override;

	long onTimer(int timer_id) override;

private:
	inline void initBrushes(void);

	inline void drawBackgroundImage(void);
	inline void drawThumbnail(void);
	inline void drawDescription(void);
	inline void drawTitle(void);
	inline void drawGenres(void);
	inline void drawYear(void);
	inline void drawDirector(void);
	inline void drawStars(void);

private:
	std::string genre_string;

	graphics::Brush m_bgImageBrush;
	graphics::Brush m_thumbnailBrush;
	graphics::Brush m_descBrush;

	Film* m_pFilm = nullptr;
	Renderer m_Renderer;
	PanelCloseButton* m_pCloseButton = nullptr;
};

