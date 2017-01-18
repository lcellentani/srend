#pragma once

#include <memory>
#include <cstdint>

#include <glm/glm.hpp>

namespace srend
{

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

	void DrawTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& c);

	bool Save(const char* filename);

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};

} // namespace srend