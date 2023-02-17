#include "GenreButton.h"
#include "win/renderer.h"

void GenreButton::setGenre(const char* genre)
{
	m_genre = genre;
}

void GenreButton::onClick(void)
{
	m_isActivated = !m_isActivated;
}

void GenreButton::draw(void)
{
	if (m_isHovered)
	{
		if (m_isActivated) {
			setBackgroundColor(0.9F, 0.9F, 0.9F);
			setOutlineColor(.8F, .8F, .8F);
		} else {
			setBackgroundColor(0.3F, 0.3F, 0.3F);
			setOutlineColor(0.3F, 0.3F, 0.3F);
		}
	}

	else
	{
		if (!m_isActivated) {
			setBackgroundColor(0.1F, 0.1F, 0.1F);
			setOutlineColor(0.3F, 0.3F, 0.3F);
		} else {
			setBackgroundColor(0.7F, 0.7F, 0.7F);
			setOutlineColor(.8F, .8F, .8F);
		}
	}

	drawBackground();

	if (m_isTextVisible)
	{
		graphics::Brush brush;

		if (m_isActivated)
		{
			brush.fill_color[0] = 0.0F;
			brush.fill_color[1] = 0.0F;
			brush.fill_color[2] = 0.0F;
		}

		Renderer renderer(this);
		renderer.drawText(10.F, 20.F, 20.F, m_genre, brush);
	}
}

void GenreButton::showText(bool show)
{
	m_isTextVisible = show;
}