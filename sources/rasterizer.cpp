#include "rasterizer.h"

#include <array>
#include <algorithm>

#include "glm\vec3.hpp"
#include "glm\geometric.hpp"

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

void Rasterizer::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const Color& color) {
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

glm::vec3 barycentric(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p) {
	//glm::vec3 a(p2.x - p0.x, p1.x - p0.x, p0.x - p.x);
	//glm::vec3 b(p2.y - p0.y, p1.y - p0.y, p0.y - p.y);
	//glm::vec3 u = glm::cross(a, b);
	//if (std::abs(u.z) < 1.0f) { return glm::vec3(-1.0f, 1.0f, 1.0f); }
	//return glm::vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);

	glm::vec3 e0(p1.x - p0.x, p1.y - p0.y, 0.0f); //
	glm::vec3 e1(p2.x - p0.x, p2.y - p0.y, 0.0f); //
	glm::vec3 e2(p.x - p0.x, p.y - p0.y, 0.0f);

	float d00 = glm::dot(e0, e0); //
	float d01 = glm::dot(e0, e1); //
	float d11 = glm::dot(e1, e1); //
	float d20 = glm::dot(e2, e0);
	float d21 = glm::dot(e2, e1);
	float inverseDenom = 1.f / (d00 * d11 - d01 * d01); //

	float v = (d11 * d20 - d01 * d21) * inverseDenom;
	float w = (d00 * d21 - d01 * d20) * inverseDenom;
	float u = 1.f - v - w;

	return glm::vec3(u, v, w);
}

void Rasterizer::DrawTriangle(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, const Color& color) {
	//const float xx = mWidth - 1.0f;
	//const float yy = mHeight - 1.0f;
	//glm::vec2 bboxMin(xx, yy);
	//glm::vec2 bboxMax(0.f, 0.f);
	//for (int i = 0; i < 3; i++) {
	//	bboxMin.x = max(0.0f, min(bboxMin.x, points[i].x));
	//	bboxMin.y = max(0.0f, min(bboxMin.y, points[i].y));
	//
	//	bboxMax.x = min(xx, max(bboxMax.x, points[i].x));
	//	bboxMax.y = min(yy, max(bboxMax.y, points[i].y));
	//}
	float maxX = max(p0.x, max(p1.x, p2.x));
	float minX = min(p0.x, min(p1.x, p2.x));
	float maxY = max(p0.y, max(p1.y, p2.y));
	float minY = min(p0.y, min(p1.y, p2.y));

	glm::vec3 e0(p1.x - p0.x, p1.y - p0.y, 0.0f);
	glm::vec3 e1(p2.x - p0.x, p2.y - p0.y, 0.0f);
	float d00 = glm::dot(e0, e0);
	float d01 = glm::dot(e0, e1);
	float d11 = glm::dot(e1, e1);
	float inverseDenom = 1.f / (d00 * d11 - d01 * d01);
	glm::vec2 p;
	for (p.x = minX; p.x <= maxX; p.x++) {
		for (p.y = minY; p.y <= maxY; p.y++) {
			glm::vec3 e2(p.x - p0.x, p.y - p0.y, 0.0f);
			float d20 = glm::dot(e2, e0);
			float d21 = glm::dot(e2, e1);
			float v = (d11 * d20 - d01 * d21) * inverseDenom;
			float w = (d00 * d21 - d01 * d20) * inverseDenom;
			float u = 1.f - v - w;
			if (u < 0 || v < 0 || w < 0) continue;
			DrawPoint((int32_t)p.x, (int32_t)p.y, color);
		}
	}
	//DrawLine({ minX, minY }, { maxX, minY }, { 0, 255, 0, 255 });
	//DrawLine({ maxX, minY }, { maxX, maxY }, { 0, 255, 0, 255 });
	//DrawLine({ maxX, maxY }, { minX, maxY }, { 0, 255, 0, 255 });
	//DrawLine({ minX, maxY }, { minX, minY }, { 0, 255, 0, 255 });

	/*
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
			DrawPoint((int32_t)x, (int32_t)y, color);
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
			DrawPoint((int32_t)x, (int32_t)y, color);
		}
	}
	*/
	//DrawLine(p0, p1, { 0, 255, 0, 255 });
	//DrawLine(p1, p2, { 0, 255, 0, 255 });
	//DrawLine(p2, p0, { 255, 0, 0, 255 });
}

} // namespace srend