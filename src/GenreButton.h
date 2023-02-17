#pragma once

#include "Button.h"

class GenreButton : public Button
{
public:
	GenreButton(const Size& size, const Point& point, Widget* pParent)
		: Button(size, point, pParent) {}

	void setGenre(const char* genre);
	void showText(bool show);

	inline std::string getGenre(void) {
		return m_genre; 
	}

	inline bool isActivated(void) {
		return m_isActivated; 
	}

	void draw(void) override;

protected:
	void onClick(void) override;

private:
	bool m_isActivated = false;
	bool m_isTextVisible = true;
	std::string m_genre = "";
};