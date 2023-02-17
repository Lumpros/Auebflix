#pragma once

#include "win/widget.h"

#include "Film.h"
#include "TextEdit.h"
#include "YearSlider.h"
#include "GenreButton.h"
#include "SearchButton.h"

#include <list>
#include <set>

class FilterControl : public Widget
{
public:
	FilterControl(const Point& point, Widget* pParent);
	~FilterControl(void);

	void draw(void) override;

	std::list<Film*> searchFilms(void);

	void showText(bool show);

private:
	static bool queryMatchesFilm(const std::string& query, const Film* pFilm);

	std::set<std::string> getSelectedGenres(void);

	void initGenreButtons(void);
	void initSliders(void);

private:
	TextEdit* m_pSearchTextEdit = nullptr;
	YearSlider* m_pFromSlider = nullptr;
	YearSlider* m_pToSlider = nullptr;
	SearchButton* m_pSearchButton = nullptr;
	GenreButton* m_genreButtons[6];

	bool m_isTextVisible = true;
};

