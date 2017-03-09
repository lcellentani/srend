#include "rasterizer.h"
#include "tgaimage.h"

#include <cassert>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

#include "shading.h"

namespace
{

static const TGAColor cMANGENTA(1.0f, 0.0f, 1.0f);

/*void Bresenham(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& image)
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
}*/

srend::vec3f Barycentric(const srend::vec3f& v0, const srend::vec3f& v1, const srend::vec3f& v2, const srend::vec3f& p) {
	srend::vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = v2[i] - v0[i];
		s[i][1] = v1[i] - v0[i];
		s[i][2] = v0[i] - p[i];
	}
	srend::vec3f u = srend::vec3f::CrossProduct(s[0], s[1]);
	if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return srend::vec3f(1.0f - (u.x() + u.y()) / u.z(), u.y() / u.z(), u.x() / u.z());
	return srend::vec3f(-1.0f, 1.0f, 1.0f);
	
	//srend::vec3f s0(v2.x() - v0.x(), v1.x() - v0.x(), v0.x() - p.x());
	//srend::vec3f s1(v2.y() - v0.y(), v1.y() - v0.y(), v0.y() - p.y());
	//srend::vec3f u = srend::vec3f::CrossProduct(s0, s1);
	//if (std::abs(u[2]) > 1e-2)
	//	return srend::vec3f(1.f - (u.x() + u.y()) / u.z(), u.y() / u.z(), u.x() / u.z());
	//return srend::vec3f(-1.0f, 1.0f, 1.0f); // triangle is degenerate, in this case return smth with negative coordinates 
}

}

namespace srend
{

struct Rasterizer::Impl
{
	TGAImage* image = nullptr;
	std::vector<float> zbuffer;
	uint32_t width;
	uint32_t height;
};

Rasterizer::Rasterizer() : mImpl(std::make_unique<Impl>()) {

}

Rasterizer::~Rasterizer() {
}

Rasterizer::Rasterizer(Rasterizer& rhs) : mImpl(std::make_unique<Impl>(*rhs.mImpl)) {

}

Rasterizer& Rasterizer::operator=(Rasterizer& rhs) {
	*mImpl = *rhs.mImpl;
	return (*this);
}

Rasterizer::Rasterizer(Rasterizer&& rhs) = default;
Rasterizer& Rasterizer::operator= (Rasterizer&& rhs) = default;

void Rasterizer::SetFramebufferSize(uint16_t width, uint16_t height) {
	if (mImpl->image) {
		delete mImpl->image;

		mImpl->zbuffer.clear();
	}

	mImpl->image = new TGAImage(width, height, TGAImage::RGB);

	int64_t size = width * height;
	for (int64_t i = size - 1; i >= 0; --i) {
		mImpl->zbuffer.push_back(-std::numeric_limits<float>::max());
	}
	
	mImpl->width = width;
	mImpl->height = height;
}

uint16_t Rasterizer::GetFramebufferWidth() const {
	return mImpl->width;
}

uint16_t Rasterizer::GetFramebufferHeight() const {
	return mImpl->height;
}

void Rasterizer::SetTexel(vec2i point, const color_rgba8& color) {
	assert(mImpl->image);
	mImpl->image->SetPixel(point.x(), point.y(), TGAColor(color.bgra(), color_rgba8::size));
}

color_rgba8 Rasterizer::GetTexel(vec2i point) const {
	assert(mImpl->image);
	TGAColor color = mImpl->image->GetPixel(point.x(), point.y());
	return color_rgba8(color.bgra);
}

void Rasterizer::DrawLine(const vec2f& p0, const vec2f& p1, const color_rgba8& color) {
	TGAColor c(color.bgra(), color_rgba8::size);

	vec2f a(p0);
	vec2f b(p1);

	bool steep = std::abs(a.x() - b.x()) < std::abs(a.y() - b.y());
	if (steep) {
		std::swap(a.x(), a.y());
		std::swap(b.x(), b.y());
	}
	if (a.x() > b.x()) {
		std::swap(a, b);
	}

	float dx = b.x() - a.x();
	float dy = std::abs(b.y() - a.y());
	float error = dx / 2.f;
	float ystep = (b.y() > a.y() ? 1.f : -1.f);
	float y = a.y();
	for (float x = a.x(); x <= b.x(); x++) {
		mImpl->image->SetPixel(steep ? y : x, steep ? x : y, c);
		error -= dy;
		if (error < 0) {
			y += ystep;
			error += dx;
		}
	}

	/*glm::vec2 a(p0);
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
	}*/
}

/*
void triangle(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i uv0, Vec2i uv1, Vec2i uv2, TGAImage &image, float intensity, int *zbuffer) {
	if (t0.y == t1.y && t0.y == t2.y) return; // i dont care about degenerate triangles
	if (t0.y>t1.y) { std::swap(t0, t1); std::swap(uv0, uv1); }
	if (t0.y>t2.y) { std::swap(t0, t2); std::swap(uv0, uv2); }
	if (t1.y>t2.y) { std::swap(t1, t2); std::swap(uv1, uv2); }

	int total_height = t2.y - t0.y;
	for (int i = 0; i<total_height; i++) {
		bool second_half = i>t1.y - t0.y || t1.y == t0.y;
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here
		Vec3i A = t0 + Vec3f(t2 - t0)*alpha;
		Vec3i B = second_half ? t1 + Vec3f(t2 - t1)*beta : t0 + Vec3f(t1 - t0)*beta;
		Vec2i uvA = uv0 + (uv2 - uv0)*alpha;
		Vec2i uvB = second_half ? uv1 + (uv2 - uv1)*beta : uv0 + (uv1 - uv0)*beta;
		if (A.x>B.x) { std::swap(A, B); std::swap(uvA, uvB); }
		for (int j = A.x; j <= B.x; j++) {
			float phi = B.x == A.x ? 1. : (float)(j - A.x) / (float)(B.x - A.x);
			Vec3i   P = Vec3f(A) + Vec3f(B - A)*phi;
			Vec2i uvP = uvA + (uvB - uvA)*phi;
			int idx = P.x + P.y*width;
			if (zbuffer[idx]<P.z) {
				zbuffer[idx] = P.z;
				TGAColor color = model->diffuse(uvP);
				image.set(P.x, P.y, TGAColor(color.r*intensity, color.g*intensity, color.b*intensity));
			}
		}
	}
}
*/

void Rasterizer::DrawTriangle(const vec2f& p0, const vec2f& p1, const vec2f& p2, ShadingFunction& shadingFunc) {
	if (p0.y() == p1.y() && p0.y() == p2.y()) return;

	vec2f t0(p0);
	vec2f t1(p1);
	vec2f t2(p2);

	if (t0.y() > t1.y()) {
		std::swap(t0, t1);
	}
	if (t0.y() > t2.y()) {
		std::swap(t0, t2);
	}
	if (t1.y() > t2.y()) {
		std::swap(t1, t2);
	}

	color_rgba8 color = shadingFunc();
	TGAColor finalColor(color.bgra(), color_rgba8::size);

	float totalHeight = t2.y() - t0.y();
	float dy0 = t1.y() - t0.y();
	float dy1 = t2.y() - t1.y();
	vec2f d_2_0 = t2 - t0;
	vec2f d_2_1 = t2 - t1;
	vec2f d_1_0 = t1 - t0;
	for (float i = 0; i < totalHeight; i++) {
		bool secondHalf = i > dy0 || t1.y() == t0.y();
		float segmentHeight = secondHalf ? dy1 : dy0;
		float alpha = i / totalHeight;
		float beta = (i - (secondHalf ? dy0 : 0)) / segmentHeight;
		vec2f A = t0 + (d_2_0 * alpha);
		vec2f B = secondHalf ? t1 + (d_2_1 * beta) : t0 + (d_1_0 * beta);
		if (A.x() > B.x()) std::swap(A, B);
		for (float j = A.x(); j <= B.x(); j++) {
			float x_ = j;
			float y_ = t0.y() + i;
			uint16_t x = static_cast<uint16_t>(std::roundf(x_));
			uint16_t y = static_cast<uint16_t>(std::roundf(y_));
			mImpl->image->SetPixel(x, y, finalColor);
		}
	}
}

void Rasterizer::DrawTriangle(const std::vector<vec3f>& pts, ShadingFunction& shadingFunc) {
	color_rgba8 color = shadingFunc();
	TGAColor finalColor(color.bgra(), color_rgba8::size);

	vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	vec2f clamp(mImpl->width - 1.0f, mImpl->height - 1.0f);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}

	vec3f p{ 0.0f, 0.0f, 0.0f };
	for (p.x() = bboxmin.x(); p.x() <= bboxmax.x(); p.x()++) {
		for (p.y() = bboxmin.y(); p.y() <= bboxmax.y(); p.y()++) {
			vec3f bc_screen = Barycentric(pts[0], pts[1], pts[2], p);
			if (bc_screen.x() < 0 || bc_screen.y() < 0 || bc_screen.z() < 0) continue;

			p.z() = 0.0f;
			for (int i = 0; i < 3; i++) p.z() += pts[i][2] * bc_screen[i];

			uint64_t idx = uint64_t(p.x() + p.y() * mImpl->width);
			if (mImpl->zbuffer[idx] < p.z()) {
				mImpl->zbuffer[idx] = p.z();
				mImpl->image->SetPixel(p.x(), p.y(), finalColor);
			}
		}
	}
}

void Rasterizer::DrawTriangle(const std::vector<Vertex>& pts, ShadingFunction& shadingFunction) {

}

/*void Rasterizer::DrawTriangle(const Vertex& p0, const Vertex& p1, const Vertex& p2, ShadingFunction& shadingFunction)
{
#if 1
	if (p0.pos.y == p1.pos.y && p0.pos.y == p2.pos.y)
		return;

	Vertex v0(p0);
	Vertex v1(p1);
	Vertex v2(p2);

	if (v0.pos.y > v1.pos.y) {
		std::swap(v0, v1);
	}
	if (v0.pos.y > v2.pos.y) {
		std::swap(v0, v2);
	}
	if (v1.pos.y > v2.pos.y) {
		std::swap(v1, v2);
	}

	float totalHeight = v2.pos.y - v0.pos.y;
	float dy0 = v1.pos.y - v0.pos.y;
	float dy1 = v2.pos.y - v1.pos.y;
	for (float i = 0; i < totalHeight; i++) {
		bool secondHalf = i > dy0 || (v1.pos.y == v0.pos.y);
		float segmentHeight = secondHalf ? dy1 : dy0;

		float alpha = i / totalHeight;
		float beta = (i - (secondHalf ? dy0 : 0)) / segmentHeight;

		Vec3f A = v0.pos + ((v2.pos - v0.pos) * alpha);
		Vec3f B = secondHalf ? (v1.pos + ((v2.pos - v1.pos) * beta)) : (v0.pos + ((v1.pos - v0.pos) * beta));
		//Vec2f uvA = v0.uv0 + ((v2.uv0 - v0.uv0) * alpha);
		//Vec2f uvB = secondHalf ? (v1.uv0 + ((v2.uv0 - v1.uv0) * beta)) : (v0.uv0 + ((v1.uv0 - v0.uv0) * beta));

		if (A.x > B.x) {
			std::swap(A, B);
			//std::swap(uvA, uvB);
		}

		float dx = B.x - A.x;
		for (int j = A.x; j <= B.x; j++) {
			//float phi = (B.x == A.x) ? 1.0f : (j - A.x) / dx;
			//Vec3f P = A + (Vec3f(B - A) * phi);
			//Vec2f uvP = uvA + ((uvB - uvA) * phi);

			//uint32_t idx = P.x + P.y * mImpl->width;
			//if (mImpl->zbuffer[idx] < P.z) {
			//	mImpl->zbuffer[idx] = P.z;
				srend::color_rgba8 finalColor = shadingFunction(Vec3f(), Vec3f(), Vec2f());
				TGAColor color(finalColor.r, finalColor.g, finalColor.b, finalColor.a);

				mImpl->image->SetPixel(j, v0.pos.y + i, color);
			//}
		}
	}
#else
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
#endif
}*/

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