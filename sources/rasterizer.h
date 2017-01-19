#pragma once

#include <memory>
#include <cstdint>

#include "glm/glm.hpp"

class TGAImage;
class ShadingFunction;

namespace srend
{

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 uv0;
	glm::vec3 norm;
};

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

	void SetTexel(uint16_t x, uint16_t y, const glm::vec4& c);
	glm::vec4 GetTexel(uint16_t x, uint16_t y) const;
	
	void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& c);

	void DrawTriangle(const Vertex& p0, const Vertex& p1, const Vertex& p2, ShadingFunction& shadingFunction);

	bool Save(const char* filename);

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};

} // namespace srend