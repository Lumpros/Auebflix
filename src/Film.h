#pragma once

#include <string>
#include <vector>
#include <list>
#include <set>

class Film
{
public:
	void setName(std::string&& name);
	void setThumbnail(std::string&& thumbnail);
	void setDescription(const std::string& description);
	void setStars(std::set<std::string>&& stars);
	void setDirector(std::string&& director);
	void setGenres(std::set<std::string>&& genres);
	void setYear(int year);

	bool hasGenres(const std::set<std::string>& genres);
	bool wasReleasedBetween(int minYear, int maxYear);

	static void loadFilms(const char* path);
	static void unloadFilms(void);
	static std::list<Film*>& getLoadedFilms(void);

	inline const std::set<std::string>& getStars(void) const noexcept
	{
		return m_stars;
	}

	inline std::string getDirector(void) const noexcept
	{
		return m_director;
	}

	/// <summary>
	/// Returns the year the film was released
	/// </summary>
	/// <returns></returns>
	inline int getYear(void) const noexcept
	{
		return m_year;
	}

	/// <summary>
	/// Returns the path of the thumbnail.
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	inline std::string getThumbnail(void)
	{
		return m_thumbnail;
	}

	/// <summary>
	/// Returns the name of the film
	/// </summary>
	/// <returns></returns>
	inline std::string getName(void) const noexcept
	{
		return m_name;
	}

	/// <summary>
	/// Returns a const reference to the set of the film's genres
	/// </summary>
	/// <returns></returns>
	inline const std::set<std::string>& getGenres(void) const noexcept
	{
		return m_genres;
	}

	/// <summary>
	/// Returns a const reference to the lines of the description of the film.
	/// </summary>
	/// <returns></returns>
	inline const std::vector<std::string>& getDescription(void) const noexcept
	{
		return m_description;
	}

private:
	std::string m_name = "";
	std::string m_thumbnail = "";
	std::string m_director = "";

	std::set<std::string> m_genres;
	std::set<std::string> m_stars;
	std::vector<std::string> m_description;

	int m_year = 0;
};
