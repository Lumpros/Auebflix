#pragma once

#include "win/widget.h"
#include "FilmButton.h"

#include <list>

#define FILM_WIDTH 200
#define FILM_HEIGHT 300

enum class FOSBDirection { LEFT, RIGHT };

class FilmOrganizerScrollButton : public Button
{
public:
	FilmOrganizerScrollButton(const Size& size, const Point& point, Widget* pWidget);

	void setDirection(FOSBDirection dir);
	void draw(void) override;

	long onMouseEnter(MouseMessageInfo* mmi);
	long onMouseLeft(MouseMessageInfo* mmi);

protected:
	void onClick(void) override;

private:
	FOSBDirection m_direction = FOSBDirection::RIGHT;
};

class FilmOrganizer : public Widget
{
public:
	FilmOrganizer(const Size& size, const Point& point, Widget* pWidget);
	~FilmOrganizer();

	void draw(void) override;

	void showText(bool show);

	long onCustom(CustomMessageInfo* info);
	long onTimer(int timer_id);
	long onMouseEnter(MouseMessageInfo* mmi);
	long onMouseLeft(MouseMessageInfo* mmi);

	void addFilm(Film* pFilm);
	void setLabel(const std::string& str);

private:
	void updateScrollButtons(void);

private:
	std::list<FilmButton*> m_FilmButtons;
	int m_distanceScrolled = 0;
	bool m_showText = true;

	std::string m_genreLabel = "";

	FilmOrganizerScrollButton* m_pLeftButton = nullptr;
	FilmOrganizerScrollButton* m_pRightButton = nullptr;
};