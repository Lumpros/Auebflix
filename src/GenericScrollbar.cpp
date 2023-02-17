#include "GenericScrollbar.h"

GenericScrollbar::GenericScrollbar(int maxScroll, Widget* pParent)
	: Scrollbar(maxScroll, pParent)
{

}

void GenericScrollbar::scroll(int dist)
{
	for (Widget* pWidget : getParent()->getChildren())
	{
		if (pWidget != this)
		{
			pWidget->setRelativePositionY(pWidget->getRelativePositionY() - dist);
		}
	}
}