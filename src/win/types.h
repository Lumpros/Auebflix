#pragma once

#include <cstdint>

typedef struct SizeU
{
	uint32_t width;
	uint32_t height;

	SizeU(void) = default;
	SizeU(uint32_t width, uint32_t height)
		: width(width), height(height) {}
} Size;

typedef struct PointI
{
	int32_t x;
	int32_t y;

	PointI(void) = default;
	PointI(int32_t x, int32_t y)
		: x(x), y(y) {}
} Point;

typedef struct RectI
{
	Point point;
	Size size;

	RectI(void) = default;
	RectI(const Point& point, const Size& size)
		: point(point), size(size) {}

	bool containsPoint(const Point& p)
	{
		return (p.x >= point.x && p.x <= point.x + static_cast<int>(size.width)) 
			&& (p.y >= point.y && p.y <= point.y + static_cast<int>(size.height));
	}
} Rect;