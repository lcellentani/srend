#include "rasterizer.h"

#include <algorithm>
using namespace std;
#ifndef max
#define max __max
#endif
#ifndef min
#define min __min
#endif

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

void Rasterizer::DrawPoint(int32_t x, int32_t y, const Color& color) {
	x = max(0, min(x, (int32_t)mWidth));
	y = max(0, min(y, (int32_t)mHeight));
	int index = (((mHeight - y) << 2) * mWidth) + (x << 2);
	mColors[index] = color.b;
	mColors[index + 1] = color.g;
	mColors[index + 2] = color.r;
	mColors[index + 3] = color.a;
}

void Rasterizer::DrawLine(glm::vec2 p0, glm::vec2 p1, const Color& color) {
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

	int32_t x0 = (int32_t)p0.x; int32_t y0 = (int32_t)p0.y;
	int32_t x1 = (int32_t)p1.x; int32_t y1 = (int32_t)p1.y;

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

void Rasterizer::DrawTriangle(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, const Color& color) {
	(void)color;
	if (p0.y > p1.y) { swap(p0, p1); }
	if (p0.y > p2.y) { swap(p0, p2); }
	if (p1.y > p2.y) { swap(p1, p2); }

	float total_height = p2.y - p0.y;
	float m0 = p1.y - p0.y + 1;
	for (float y = p0.y; y <= p1.y; y++) {
		float alpha = (float)(y - p0.y) / total_height;
		float beta = (float)(y - p0.y) / m0;
		float A = p0.x + (p2.x - p0.x) * alpha;
		float B = p0.x + (p1.x - p0.x) * beta;
		if (A > B) { swap(A, B); }
		for (float x = A; x <= B; x++) {
			DrawPoint((int32_t)x, (int32_t)y, { 255,255,255,255 });
		}
	}
	float m1 = p2.y - p1.y + 1;
	for (float y = p1.y; y <= p2.y; y++) {
		float alpha = (float)(y - p0.y) / total_height;
		float beta = (float)(y - p1.y) / m1;
		float A = p0.x + (p2.x - p0.x) * alpha;
		float B = p1.x + (p2.x - p1.x) * beta;
		if (A > B) { swap(A, B); }
		for (float x = A; x <= B; x++) {
			DrawPoint((int32_t)x, (int32_t)y, { 255,255,255,255 });
		}
	}

	DrawLine(p0, p1, { 0, 255, 0, 255 });
	DrawLine(p1, p2, { 0, 255, 0, 255 });
	DrawLine(p2, p0, { 255, 0, 0, 255 });
}

} // namespace srend