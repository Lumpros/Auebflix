#pragma once

#include "Button.h"
#include "Film.h"
#include "win/renderer.h"

#define HIDE_DRAWN_TEXT 100
#define SHOW_RESULT_TEXT 110

class FilmButton : public Button
{
public:
	FilmButton(const Size& size, const Point& point, Widget* pParent)
		: Button(size, point, pParent), m_Renderer(this) 
	{
		m_bgBrush.outline_opacity = 0.0F;
	}

	void draw(void) override;
	void setFilm(Film* pFilm);

	long onMouseEnter(MouseMessageInfo* mmi) override;
	long onMouseLeft(MouseMessageInfo* mmi) override;
	long onTimer(int timer_id) override;

protected:
	void onClick(void);

private:
	Renderer m_Renderer;

	Film* m_pFilm = {};

	int m_extraSize = 0;
};

