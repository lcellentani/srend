#include "rasterizer.h"
#include "tgaimage.h"

#include <cassert>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

namespace
{

const TGAColor cMANGENTA(255.0f, 0.0f, 255.0f, 255.0f);

void Bresenham(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& image)
{
#if _DEBUG
	image.SetPixel(x0, y0, cMANGENTA);
	image.SetPixel(x1, y1, cMANGENTA);
#endif

	bool steep = std::abs(x0 - x1) < std::abs(y0 - y1);
	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = std::abs(y1 - y0);
	int error = dx >> 1;
	int ystep = (y1 > y0 ? 1 : -1);
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		image.SetPixel(steep ? y : x, steep ? x : y, color);
		error -= dy;
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}
}

glm::vec3 Barycentric(const glm::vec2&v0, const glm::vec2&v1, const glm::vec2&v2, const glm::vec2& p) {
	glm::vec3 s0(v2.x - v0.x, v1.x - v0.x, v0.x - p.x);
	glm::vec3 s1(v2.y - v0.y, v1.y - v0.y, v0.y - p.y);
	glm::vec3 u = glm::cross(s0, s1);
	if (std::abs(u[2]) > 1e-2)
		return glm::vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return glm::vec3(-1.0f, 1.0f, 1.0f); // triangle is degenerate, in this case return smth with negative coordinates 
}

}

namespace srend
{

struct Rasterizer::Impl
{
	TGAImage* image = nullptr;
	std::vector<float> zbuffer;
	uint16_t width;
	uint16_t height;
};

Rasterizer::Rasterizer() : mImpl(std::make_unique<Impl>())
{

}

Rasterizer::~Rasterizer()
{
}

Rasterizer::Rasterizer(Rasterizer& rhs) : mImpl(std::make_unique<Impl>(*rhs.mImpl))
{

}

Rasterizer& Rasterizer::operator=(Rasterizer& rhs)
{
	*mImpl = *rhs.mImpl;
	return (*this);
}

Rasterizer::Rasterizer(Rasterizer&& rhs) = default;
Rasterizer& Rasterizer::operator= (Rasterizer&& rhs) = default;

void Rasterizer::SetFramebufferSize(uint16_t width, uint16_t height)
{
	if (mImpl->image) {
		delete mImpl->image;
	}

	mImpl->image = new TGAImage(width, height, TGAImage::RGB);

	uint64_t size = width * height;
	mImpl->zbuffer.reserve(size);
	for (int i = size - 1; i >= 0; --i) {
		mImpl->zbuffer.push_back(-std::numeric_limits<float>::max());
	}
	//std::fill(mImpl->zbuffer.begin(), mImpl->zbuffer.end(), -std::numeric_limits<float>::max());
	
	mImpl->width = width;
	mImpl->height = height;
}

uint16_t Rasterizer::GetFramebufferWidth() const {
	return mImpl->width;
}

uint16_t Rasterizer::GetFramebufferHeight() const {
	return mImpl->height;
}

void Rasterizer::SetTexel(uint16_t x, uint16_t y, const glm::vec4& c)
{
	assert(mImpl->image);
	mImpl->image->SetPixel(x, y, TGAColor(c.r, c.g, c.b, c.a));
}

glm::vec4 Rasterizer::GetTexel(uint16_t x, uint16_t y) const {
	assert(mImpl->image);
	TGAColor c = mImpl->image->GetPixel(x, y);
	return glm::vec4(c.bgra[2] / 255.0f, c.bgra[1] / 255.0f, c.bgra[0] / 255.0f, c.bgra[3] / 255.0f);
}

void Rasterizer::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& c)
{
	/*
#if _DEBUG
	mImpl->image->SetPixel(x0, y0, cMANGENTA);
	mImpl->image->SetPixel(x1, y1, cMANGENTA);
#endif

	bool steep = std::abs(x0 - x1) < std::abs(y0 - y1);
	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = std::abs(y1 - y0);
	int error = dx >> 1;
	int ystep = (y1 > y0 ? 1 : -1);
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		mImpl->image->SetPixel(steep ? y : x, steep ? x : y, TGAColor(c.r, c.g, c.b, c.a));
		error -= dy;
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}
	*/

	glm::vec2 a(p0);
	glm::vec2 b(p1);
	TGAColor color(c.r, c.g, c.b, c.a);

	bool steep = std::abs(a.x - b.x) < std::abs(a.y - b.y);
	if (steep) {
		std::swap(a.x, a.y);
		std::swap(b.x, b.y);
	}
	if (a.x > b.x) {
		std::swap(a.x, b.x);
		std::swap(a.y, b.y);
	}

	float dx = b.x - a.x;
	float dy = b.y - a.y;
	float gradient = dy / dx;

	int xx, yy;

	xx = static_cast<int>(steep ? a.y : a.x);
	yy = static_cast<int>(steep ? a.x : a.y);
	mImpl->image->SetPixel(xx, yy, color);

	xx = static_cast<int>(steep ? b.y : b.x);
	yy = static_cast<int>(steep ? b.x : b.y);
	mImpl->image->SetPixel(xx, yy, color);

	float y = a.y + gradient;
	float x0 = a.x;
	float x1 = b.x;
	for (float x = x0 + 1; x <= x1 - 1; x++) {
		float a2 = y - ((int)y);
		float a1 = 1.0f - a2;
		TGAColor c1, c2;
		c1 = color * a1;
		c2 = color * a2;

		xx = static_cast<int>(std::floor(x));
		yy = static_cast<int>(std::floor(y));
		int yy1 = yy + 1;
		mImpl->image->SetPixel(steep ? yy : xx, steep ? xx : yy, c1);
		mImpl->image->SetPixel(steep ? yy1 : xx, steep ? xx : yy1, c2);

		y += gradient;
	}
}

void Rasterizer::DrawTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c)
{
	TGAColor color(c.r, c.g, c.b, c.a);

	float minX = std::min(std::min(p0.x, p1.x), p2.x);
	float minY = std::min(std::min(p0.y, p1.y), p2.y);
	float maxX = std::max(std::max(p0.x, p1.x), p2.x);
	float maxY = std::max(std::max(p0.y, p1.y), p2.y);

	minX = std::max(minX, 0.0f);
	minY = std::max(minY, 0.0f);
	maxX = std::min(maxX, mImpl->width - 1.0f);
	maxY = std::min(maxY, mImpl->height - 1.0f);

	glm::vec3 p(0.0f, 0.0f, 0.0f);
	for (p.x = minX; p.x <= maxX; p.x++) {
		for (p.y = minY; p.y <= maxY; p.y++) {
			glm::vec3 bc_screen = Barycentric(p0, p1, p2, p);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			
			p.z = 0.0f;
			p.z += p0.z * bc_screen[0];
			p.z += p1.z * bc_screen[1];
			p.z += p2.z * bc_screen[2];

			uint64_t idx = int(p.x + p.y * mImpl->width);
			if (mImpl->zbuffer[idx] < p.z) {
				mImpl->zbuffer[idx] = p.z;
				mImpl->image->SetPixel(p.x, p.y, color);
			}
		}
	}
}

bool Rasterizer::Save(const char* filename)
{
	assert(mImpl->image);
	if (!filename) {
		return false;
	}

	mImpl->image->FlipVertically();
	return mImpl->image->Write(filename);
}

} // srend