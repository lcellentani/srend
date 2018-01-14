#include "rasterizer.h"

namespace srend
{

Rasterizer::Rasterizer() {

}

Rasterizer::~Rasterizer() {

}

void Rasterizer::SetViewport(uint32_t width, uint32_t height) {
	mWidth = width;
	mHeight = height;
	uint32_t  size = mWidth * mHeight * 4;
	mColors = std::vector<uint8_t>(size, 0);
	for (uint32_t i = 0; i < size; i += 4) {
		mColors[i] = 0;
		mColors[i + 1] = 0;
		mColors[i + 2] = 0;
		mColors[i + 3] = 255;
	}
}
const std::vector<uint8_t>& Rasterizer::GetColorBuffer() const {
	return mColors;
}

void Rasterizer::DrawPoint(uint32_t x, uint32_t y, const Color& color) {
	int index = (((mHeight - y) << 2) * mWidth) + (x << 2);
	mColors[index] = color.b;
	mColors[index + 1] = color.g;
	mColors[index + 2] = color.r;
	mColors[index + 3] = color.a;
}

void Rasterizer::DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, const Color& color) {
	/*bool transposed = std::abs(x0 - x1) < std::abs(y0 - y1);
	if (transposed) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	float dx = (float)(x1 - x0);
	for (int32_t x = x0; x <= x1; x++) {
		float t = (x - x0) / dx;
		uint32_t y = (uint32_t)(y0 * (1.f - t) + y1 * t);
		if (transposed) {
			DrawPoint(y, x, color);
		} else {
			DrawPoint(x, y, color);
		}
	}*/

	int dy = y1 - y0;
	int dx = x1 - x0;

	int stepy = 1;
	if (dy < 0) { dy = -dy;  stepy = -1; }
	int stepx = 1;
	if (dx < 0) { dx = -dx;  stepx = -1; }

	dy <<= 1; // dy is now 2*dy
	dx <<= 1; // dx is now 2*dx

	DrawPoint(x0, y0, color);
	if (dx > dy) {
		int fraction = dy - (dx >> 1); // same as 2*dy - dx
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx; // same as fraction -= 2*dx
			}
			x0 += stepx;
			fraction += dy; // same as fraction += 2*dy
			DrawPoint(x0, y0, color);
		}
	} else {
		int fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			DrawPoint(x0, y0, color);
		}
	}
}

} // namespace srend