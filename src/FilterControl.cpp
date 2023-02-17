#include "FilterControl.h"
#include "Film.h"

#include <set>
#include <string>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

FilterControl::FilterControl(const Point& point, Widget* pParent)
	: Widget(Size(340, 360), point, pParent)
{
	constexpr int padding = 20;

	setBackgroundColor(0.0F, 0.0F, 0.F);
	setOutlineColor(0.3F, 0.0F, 0.0F);
	setOpacity(0.8F);

	m_pSearchTextEdit = new TextEdit(Size(300, 40), Point(padding, padding), this);
	m_pSearchTextEdit->setPlaceholderText("Film Title/Protagonist/Director");

	m_pSearchButton = new SearchButton(Size(300, 50), Point(padding, padding + 270), this);

	initSliders();
	initGenreButtons();
}

FilterControl::~FilterControl(void)
{
	delete m_pSearchTextEdit;
	delete m_pToSlider;
	delete m_pFromSlider;
	delete m_pSearchButton;

	for (GenreButton* pGenreButton : m_genreButtons)
	{
		delete pGenreButton;
	}
}

void FilterControl::initGenreButtons(void)
{
	const Size genreButtonSizes[] = {
		Size(110, 25),
		Size(85, 25),
		Size(58, 25),
		Size(90, 25),
		Size(75, 25),
		Size(80, 25)
	};

	const char* genres[] = {
		"Adventure",
		"Fantasy",
		"SciFi",
		"Comedy",
		"Drama",
		"Action"
	};

	for (int i = 0; i < ARRAY_SIZE(genres); ++i)
	{
		m_genreButtons[i] = new GenreButton(genreButtonSizes[i], Point(20, 70 + (i / 3) * 35), this);
		m_genreButtons[i]->setGenre(genres[i]);

		if (i % 3 != 0)
		{
			m_genreButtons[i]->setRelativePositionX(m_genreButtons[i - 1]->getRelativePositionX() + m_genreButtons[i - 1]->getWidth() + 20);
		}
	}
}

void FilterControl::initSliders(void)
{
	m_pFromSlider = new YearSlider(Size(220, 35), Point(100, 170), this);
	m_pFromSlider->setRange(1979, 2022);

	m_pToSlider = new YearSlider(Size(220, 35), Point(100, 245), this);
	m_pToSlider->setRange(1979, 2022);
	m_pToSlider->setValue(2022);
}

/// <summary>
/// Using the filters selected by the user, this function searches through the loaded films and finds
/// all the films matching the given criteria. Afterwards, a SearchResultPanel is created that displays
/// all the movies that were found. If no movies were found, nothing happens.
/// </summary>
/// <param name=""></param>
/// <return></return>
std::list<Film*> FilterControl::searchFilms(void)
{
	int minYear = m_pFromSlider->getValue();
	int maxYear = m_pToSlider->getValue();

	std::string query = m_pSearchTextEdit->getText();

	// Convert the search query to all lower case characters in order to make comparisons later on.
	std::transform(query.begin(), query.end(), query.begin(), ::tolower);

	// Gets the name of the genres that have been selected by clicking the matching genre buttons
	const std::set<std::string> genres = getSelectedGenres();

	// We'll save the movies that satisfy all the given criteria in this vector
	std::list<Film*> relevantFilms;

	for (Film* pFilm : Film::getLoadedFilms())
	{
		if (pFilm->wasReleasedBetween(minYear, maxYear) && pFilm->hasGenres(genres) && queryMatchesFilm(query, pFilm))
		{
			relevantFilms.emplace_back(pFilm);
		}
	}

	return relevantFilms;
}

/// <summary>
/// Determines whether the film given should appear in the search results given the query.
/// </summary>
/// <param name="query">Search query in LOWERCASE</param>
/// <param name="pFilm">Pointer to film</param>
/// <returns></returns>
bool FilterControl::queryMatchesFilm(const std::string& query, const Film* pFilm)
{
	// If the query was empty then all films match
	if (query == "")
	{
		return true;
	}

	std::string filmTitle = pFilm->getName();

	// We convert the title to all lowercase characters in order to compare it to the
	// query which was also converted to all lowercase characters prior to calling this function.
	std::transform(filmTitle.begin(), filmTitle.end(), filmTitle.begin(), ::tolower);
	
	if (filmTitle.find(query) == std::string::npos)
	{
		for (std::string protagonist : pFilm->getStars())
		{
			std::transform(protagonist.begin(), protagonist.end(), protagonist.begin(), ::tolower);

			if (protagonist.find(query) != std::string::npos)
			{
				return true;
			}
		}

		std::string director = pFilm->getDirector();
		std::transform(director.begin(), director.end(), director.begin(), ::tolower);

		if (director.find(query) == std::string::npos)
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// Checks which genre buttons are activated and adds their tag to a set.
/// </summary>
/// <returns>A set containing the genres that have been selected in the filtering section</returns>
std::set<std::string> FilterControl::getSelectedGenres(void)
{
	std::set<std::string> selectedGenres;

	for (GenreButton* pGenreButton : m_genreButtons)
	{
		if (pGenreButton->isActivated())
		{
			selectedGenres.insert(pGenreButton->getGenre());
		}
	}

	return selectedGenres;
}

void FilterControl::showText(bool show)
{
	m_isTextVisible = show;

	for (GenreButton* pButton : m_genreButtons)
	{
		pButton->showText(show);
	}

	m_pToSlider->showText(show);
	m_pFromSlider->showText(show);
	m_pSearchTextEdit->showText(show);
	m_pSearchButton->showText(show);
}

void FilterControl::draw(void)
{
	drawBackground();

	if (m_isTextVisible)
	{
		graphics::Brush brush;

		Renderer renderer(this);
		renderer.drawText(30, 195, 25, "From", brush);
		renderer.drawText(50, 265, 25, "To", brush);
	}

	drawChildren();
}