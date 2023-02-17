#pragma once

#include "Scrollbar.h"

class GenericScrollbar : public Scrollbar
{
public:
	GenericScrollbar(int maxScroll, Widget* pParent);

protected:
	void scroll(int dist) override;
};

