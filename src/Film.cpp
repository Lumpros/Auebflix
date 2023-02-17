#include "Film.h"
#include "FilmParser.h"

#include <algorithm>
#include <list>

static std::list<Film*> g_loadedFilms;

/// <summary>
/// Sets the film's name.
/// </summary>
/// <param name="name">The film's name</param>
void Film::setName(std::string&& name)
{
	m_name = std::move(name);
}

void Film::setStars(std::set<std::string>&& stars)
{
	m_stars = std::move(stars);
}

void Film::setDirector(std::string&& director)
{
	m_director = std::move(director);
}

/// <summary>
/// Sets the path of the image that will be displayed as the film thumbnail.
/// </summary>
/// <param name="thumbnail">Path to thumbnail image</param>
/// <returns></returns>
void Film::setThumbnail(std::string&& thumbnail)
{
	m_thumbnail = std::move(thumbnail);
}

/// <summary>
/// Breaks down the given description to lines.
/// This is done because the sgg library refuses to cooperate with new-line characters so we have to do this process ourselves.
/// </summary>
/// <param name="description"></param>
void Film::setDescription(const std::string& description)
{
	const size_t descriptionLength = description.length();

	constexpr int lineLength = 80;

	int lastNewLine = 0;

	for (int i = 0; i < descriptionLength; ++i)
	{
		if ((i + 1) % lineLength == 0 || i == descriptionLength - 1)
		{
			std::string line = description.substr(lastNewLine, i - lastNewLine);

			if (i < descriptionLength - 1 && description[i] != ' ')
			{
				line += '-';
			}

			lastNewLine = i;

			m_description.emplace_back(std::move(line));
		}
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="genres"></param>
void Film::setGenres(std::set<std::string>&& genres)
{
	m_genres = std::move(genres);
}

/// <summary>
/// Sets the year the film was released
/// </summary>
/// <param name="year"></param>
void Film::setYear(int year)
{
	m_year = year;
}

/// <summary>
/// Checks whether the film has all the given genres.
/// </summary>
/// <param name="genres"></param>
/// <returns>True if the film's genre set is a superset of the given genres, otherwise returns false </returns>
bool Film::hasGenres(const std::set<std::string>& genres)
{
	if (genres.empty())
	{
		return true;
	}

	return std::includes(m_genres.begin(), m_genres.end(), genres.begin(), genres.end());
}

/// <summary>
/// Checks whether the film was released between the given years.
/// </summary>
/// <param name="minYear">The year that came first</param>
/// <param name="maxYear">The year that came after</param>
/// <returns></returns>
bool Film::wasReleasedBetween(int minYear, int maxYear)
{
	if (minYear > maxYear)
	{
		std::swap(minYear, maxYear);
	}

	return minYear <= m_year && m_year <= maxYear;
}

/// <summary>
/// Loads films from file.
/// </summary>
/// <param name="path">Path to the file that contains the film information</param>
void Film::loadFilms(const char* path)
{
	ParsedFilmInfo info;
	FilmParser parser(path);

	while (parser.hasMoreFilms())
	{
		parser.getNextFilmInformation(&info);

		Film* pFilm = new Film();
		pFilm->setName(std::move(info.title));
		pFilm->setGenres(std::move(info.genres));
		pFilm->setDescription(info.description);
		pFilm->setYear(std::stoi(info.year));
		pFilm->setThumbnail(std::move("assets\\" + info.thumbnail));
		pFilm->setStars(std::move(info.stars));
		pFilm->setDirector(std::move(info.director));
		g_loadedFilms.emplace_back(pFilm);
	}
}

/// <summary>
/// Deletes the pointers in the loaded film set and empties the set afterwards.
/// </summary>
void Film::unloadFilms(void)
{
	for (Film* pFilm : g_loadedFilms)
	{
		delete pFilm;
	}

	g_loadedFilms.clear();
}

// Returns a reference to the set containing the loaded films.
std::list<Film*>& Film::getLoadedFilms(void)
{
	return g_loadedFilms;
}