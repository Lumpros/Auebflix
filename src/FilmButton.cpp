#include "FilmButton.h"
#include "FilmInfoPanel.h"
#include "FilmOrganizer.h"

#include <cassert>

#define SCALE_UP_TIMER 100
#define SCALE_DOWN_TIMER 101

void FilmButton::draw(void)
{
	const int width = static_cast<int>(getWidth());
	const int height = static_cast<int>(getHeight());

	// We can't just increment the width and the height by the same number
	// because the ratio of the image would be altered and it would look weird
	float ratio = height / (float)width;

	m_Renderer.drawRect(
		width / 2.F,
		height / 2.F,
		static_cast<float>(width + m_extraSize),
		height + m_extraSize * ratio,
		m_bgBrush
	);
}

void FilmButton::setFilm(Film* pFilm)
{
	m_pFilm = pFilm;
	m_bgBrush.texture = m_pFilm->getThumbnail();
	m_bgBrush.outline_opacity = 0.0F;
}

/// <summary>
/// Increases the m_extraSize variable when it receives a scale up timer message,
/// otherwise it decreases it. The timers are killed in here once the size has reached a certain point
/// </summary>
/// <param name="timer_id"></param>
/// <returns></returns>
long FilmButton::onTimer(int timer_id)
{
	if (timer_id == SCALE_UP_TIMER)
	{
		if (m_extraSize == 20)  {
			killTimer(SCALE_UP_TIMER);
		} else {
			m_extraSize += 5;
		}
	}

	else
	{
		if (m_extraSize == 0) {
			killTimer(SCALE_DOWN_TIMER);
		} else {
			m_extraSize -= 5;
		}
	}

	return 0L;
}

long FilmButton::onMouseEnter(MouseMessageInfo* mmi)
{
	killTimer(SCALE_DOWN_TIMER);
	addTimer(SCALE_UP_TIMER, 20);

	// We want to let the parent know that the cursor is inside its
	// client area so that it shows the scroll right/left buttons.
	getParent()->onMouseEnter(mmi);

	return 0L;
}

long FilmButton::onMouseLeft(MouseMessageInfo* mmi)
{
	killTimer(SCALE_UP_TIMER);
	addTimer(SCALE_DOWN_TIMER, 20);

	Widget* pParent = getParent();

	// If the mouse left the film button, then there are 2 possibilities:
	// 1. The cursor entered the FilmOrganizer client area, which means that
	//    it shouldn't receive a mouse left message and therefore hide the scroll buttons
	// 2. The cursor is now outside the FilmOrganizer client area, but since the cursor
	//    was inside a FilmButton, the FilmOrganizer won't receive a mouse left message,
	//    so we have to call the function ourselves.
	if (Widget::getCursorWidget() != pParent)
	{
		getParent()->onMouseLeft(mmi);
	}

	return 0L;
}

/// <summary>
/// Creates a FilmInfoPanel object displaying the information held in the film structure,
/// and sends a CUSTOM message to the main window telling it to hide the organizer labels,
/// because they're drawn on top of EVERYTHING for some god forsaken reason.
/// </summary>
/// <returns>Return code</returns>
void FilmButton::onClick(void)
{
	// This is deleted in AppWindow::onCustom
	new FilmInfoPanel(m_pFilm, getRoot());

	CustomMessageInfo* pInfo = new CustomMessageInfo;
	pInfo->id = HIDE_DRAWN_TEXT;
	pInfo->data = nullptr;

	Message message = {};
	message.code = Message::Code::CUSTOM;
	message.data = pInfo;
	getRoot()->postMessage(message);


	pInfo = new CustomMessageInfo;
	pInfo->id = SHOW_RESULT_TEXT;
	pInfo->data = (void*)false;

	message.code = Message::Code::CUSTOM;
	message.data = pInfo;
	getParent()->postMessage(message);
}