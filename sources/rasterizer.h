#pragma once

#include <memory>
#include <cstdint>

#include "vector.h"

class TGAImage;
class ShadingFunction;

namespace srend
{

//struct Vertex
//{
//	Vec3f pos;
//	Vec2f uv0;
//	Vec3f norm;
//};

class Rasterizer
{
public:
	Rasterizer();
	~Rasterizer();

	Rasterizer(Rasterizer& rhs);
	Rasterizer& operator=(Rasterizer& rhs);

	Rasterizer(Rasterizer&& rhs);
	Rasterizer& operator= (Rasterizer&& rhs);

	void SetFramebufferSize(uint16_t width, uint16_t height);
	uint16_t GetFramebufferWidth() const;
	uint16_t GetFramebufferHeight() const;

	void SetTexel(vec2i point, const color_rgba8& color);
	color_rgba8 GetTexel(vec2i point) const;
	
	void DrawLine(const vec2f& p0, const vec2f& p1, const color_rgba8& color);

	void DrawTriangle(vec2f& p0, vec2f& p1, vec2f& p2, ShadingFunction& shadingFunc);
	//void DrawTriangle(const Vertex& p0, const Vertex& p1, const Vertex& p2, ShadingFunction& shadingFunction);

	bool Save(const char* filename);

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};

} // namespace srend