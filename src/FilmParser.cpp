#include "FilmParser.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

#define INVALID_LINE (-1)
#define INVALID_COLUMN (-1)

/// <summary>
/// Enum class that represents what the parser is currently doing inside the parse function.
/// </summary>
enum class ParserState
{
	IDLE,
	READING_INFO,
	READING_TITLE,
	READING_DESCRIPTION,
	READING_GENRES,
	READING_THUMBNAIL_PATH,
	READING_YEAR,
	READING_COLON_CHAR,
	READING_STARS,
	READING_DIRECTOR
};

/// <summary>
/// Structure used to represent a token.
/// Contains the string representation of the token
/// as well as the line and column where it was found.
/// </summary>
struct Token
{
	std::string token = "";
	int line = INVALID_LINE;
	int column = INVALID_COLUMN;
};

/// <summary>
/// This class will be used to break the input file down into tokens
/// and provide them one by one to the FilmParser
/// </summary>
class FilmTokenizer
{
public:
	void tokenize(std::fstream& in);

	bool areTokensLeft(void) const noexcept;
	Token getNextToken(void) noexcept;

private:
	void tokenizeLine(const std::string& line, int lineNo);
	void addToken(const std::string& token, int line, int column);

private:
	int m_tokenIndex = 0;
	std::vector<Token> m_tokens;
};

/// <summary>
/// Clears all the information contained in the structure.
/// Used in preparation of reading the next film's information
/// during the parsing phase of the program.
/// </summary>
void ParsedFilmInfo::clear(void)
{
	title = "";
	description = "";
	thumbnail = "";
	year = "";
	director = "";

	stars.clear();
	genres.clear();
}

/// <summary>
/// Checks whether getNextToken() can be called again
/// </summary>
bool FilmTokenizer::areTokensLeft(void) const noexcept
{
	return m_tokenIndex < m_tokens.size();
}

/// <summary>
/// Returns the next token in the token stream.
/// If areTokensLeft() returns false and this function is called
/// an exception will be thrown.
/// </summary>
/// <returns>The next token in the stream</returns>
Token FilmTokenizer::getNextToken(void) noexcept
{
	return m_tokens[m_tokenIndex++];
}

/// <summary>
/// Breaks down the text read from the input stream down to tokens
/// The tokens can be retrieved using the getNextToken function.
/// </summary>
/// <param name="in"></param>
void FilmTokenizer::tokenize(std::fstream& in)
{
	m_tokenIndex = 0;

	int currentLineNumber = 1;
	std::string line;

	while (std::getline(in, line))
	{
		tokenizeLine(line, currentLineNumber++);
	}
}

void FilmTokenizer::tokenizeLine(const std::string& line, int lineNo)
{
	const int lineLength = static_cast<int>(line.length());

	std::string token = "";

	for (int i = 0; i < lineLength; ++i)
	{
		// If we find a whitespace character we'll check if we've been reading a token
		// If we have, then we consider the token finished and we add it to the vector
		// If we haven't, that means we're looking for the next token so we keep going
		if (isspace(line[i]))
		{
			if (!token.empty())
			{
				addToken(token, lineNo, i + 1);
				token.clear();
			}
		}

		// If we find a ':' character, we add 3 tokens to the vector:
		// Lets say for example that the line was 'TITLE: Lord of the Rings'
		// The first token will be everything before the ':' character, in this case 'TITLE'
		// The second token will be the ':' character
		// The third token will be everything after the ':' character, in this case 'Lord of the Rings's
		else if (line[i] == ':')
		{
			// We add 1 to i because we want the first index to be 1 not 0
			// Additionally, we subtract the token length here in order to get the
			// index of the first character, 
			addToken(token, lineNo, (i + 1) - (int)token.length());
			addToken(":", lineNo, i + 1);

			size_t first = line.find_first_not_of(' ', i + 1);
			size_t last = line.find_last_not_of(' ');

			if (first == std::string::npos)
			{
				addToken("", lineNo, i + 2);
			}

			else if (last != std::string::npos)
			{
				addToken(line.substr(first, last - first + 1), lineNo, i + 2);
			}

			break;
		}

		else
		{
			// If the character isn't a whitespace or ':', we add it to the token
			token += line[i];

			// If we've reached the end of the line, then we add the token
			// to the list, otherwise it will be left out
			if (i == lineLength - 1)
			{
				addToken(token, lineNo, (i + 1) - (int)token.length());
			}
		}
	}
}

/// <summary>
/// Adds a token to the m_tokens vector with the given parameters
/// </summary>
/// <param name="token">String representaion of the token</param>
/// <param name="line">Line where the token was found</param>
/// <param name="column">Column of the line where the token was found</param>
void FilmTokenizer::addToken(const std::string& token, int line, int column)
{
	Token tok;
	tok.token = token;
	tok.line = line;
	tok.column = column;

	m_tokens.emplace_back(tok);
}

FilmParser::FilmParser(const char* path)
{
	file.open(path);

	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open '" + std::string(path) + "'");
	}

	parse();
}

/// <summary>
/// Splits the string around the given delimiter
/// </summary>
/// <param name="s">The string that will be split</param>
/// <param name="delimiter"></param>
/// <returns>A vector containing the tokens in order</returns>
static std::vector<std::string> split(std::string s, std::string delimiter) 
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) 
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

void FilmParser::parse(void)
{
	FilmTokenizer tokenizer;
	tokenizer.tokenize(file);

	ParserState state = ParserState::IDLE;

	// We use this variable before entering the READING_COLON_CHAR segment in order to know which state comes next.
	ParserState nextState;
	ParsedFilmInfo info;
	
	while (tokenizer.areTokensLeft())
	{
		Token token = tokenizer.getNextToken();
		
		switch (state)
		{
		case ParserState::IDLE:
			// When the parser is idle and it encounters the START keyword, we
			// begin reading the film information (attributes)
			if (token.token == "START") {
				state = ParserState::READING_INFO;
			} else {
				throw std::runtime_error("Was expecting START, got " + token.token + " instead (" + std::to_string(token.line) + std::to_string(token.column) + ")");
			}
			break;

		case ParserState::READING_INFO:
			if (token.token == "END") {
				state = ParserState::IDLE;
				m_parsedInfo.emplace_back(info);
				info.clear();
			} else {
				state = ParserState::READING_COLON_CHAR;
				if (token.token == "TITLE") {
					nextState = ParserState::READING_TITLE;
				} else if (token.token == "DESCRIPTION") {
					nextState = ParserState::READING_DESCRIPTION;
				} else if (token.token == "YEAR") {
					nextState = ParserState::READING_YEAR;
				} else if (token.token == "GENRE") {
					nextState = ParserState::READING_GENRES;
				} else if (token.token == "THUMBNAIL") {
					nextState = ParserState::READING_THUMBNAIL_PATH;
				} else if (token.token == "DIRECTOR") {
					nextState = ParserState::READING_DIRECTOR;
				} else if (token.token == "STARS") {
					nextState = ParserState::READING_STARS;
				} else {
					throw std::runtime_error("Unknown attribute " + token.token + "found (" + std::to_string(token.line) + ", " + std::to_string(token.column) + ")");
				}
			}
			break;

		case ParserState::READING_COLON_CHAR:
			if (token.token != ":") {
				throw std::runtime_error("Was expecting a colon ':', got " + token.token + " instead (" + std::to_string(token.line) + ", " + std::to_string(token.column) + ")");
			} 
			state = nextState;
			break;

		case ParserState::READING_TITLE:
			info.title = token.token;
			state = ParserState::READING_INFO;
			break;

		case ParserState::READING_DESCRIPTION:
			info.description = token.token;
			state = ParserState::READING_INFO;
			break;

		case ParserState::READING_YEAR:
			info.year = token.token;
			state = ParserState::READING_INFO;
			break;

		case ParserState::READING_GENRES: {
			std::vector<std::string> genres = split(token.token, ", ");
			for (std::string& genre : genres) {
				info.genres.insert(genre);
			}
			state = ParserState::READING_INFO;
			break;
		}

		case ParserState::READING_THUMBNAIL_PATH:
			info.thumbnail = token.token;
			state = ParserState::READING_INFO;
			break;

		case ParserState::READING_DIRECTOR:
			info.director = token.token;
			state = ParserState::READING_INFO;
			break;

		case ParserState::READING_STARS: {
			std::vector<std::string> stars = split(token.token, ", ");
			for (std::string& star : stars) {
				info.stars.insert(star);
			}
			state = ParserState::READING_INFO;
			break;
		}
		}
	}

	if (state != ParserState::IDLE)
	{
		throw std::runtime_error("Unexpected end of file reached");
	}
}

/// <summary>
/// Checks whether getNextFilmInformation() can be called
/// </summary>
/// <returns></returns>
bool FilmParser::hasMoreFilms(void) const noexcept
{
	return infoIndex < m_parsedInfo.size();
}

/// <summary>
/// Fills the given structure with the information of the next film in the file.
/// If getNextFilmInformation() returns false, an exception is thrown
/// </summary>
/// <param name="out">Pointer to a ParsedFilmInfo struct that receives the information</param>
void FilmParser::getNextFilmInformation(ParsedFilmInfo* out)
{
	assert(out);

	*out = m_parsedInfo[infoIndex++];
}