#pragma once

#include <string>
#include <set>
#include <fstream>
#include <vector>

struct ParsedFilmInfo
{
	std::string title = "";
	std::string description = "";
	std::string thumbnail = "";
	std::string year = "";
	std::string director = "";

	std::set<std::string> stars;
	std::set<std::string> genres;

	void clear();
};

class FilmParser
{
public:
	FilmParser(const char* path);

	bool hasMoreFilms(void) const noexcept;

	void getNextFilmInformation(ParsedFilmInfo* out);

private:
	void parse(void);

private:
	std::fstream file;
	std::vector<ParsedFilmInfo> m_parsedInfo;

	int infoIndex = 0;
};

