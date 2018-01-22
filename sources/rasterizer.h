#pragma once

#include <cstdint>
#include <vector>

#include "glm/vec2.hpp"

namespace srend
{

union Color {
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
	uint8_t data[4];
};

class Rasterizer final {
public:
	Rasterizer();
	~Rasterizer();

	void SetViewport(uint32_t width, uint32_t height);

	const std::vector<uint8_t>& GetColorBuffer() const;

	void DrawPoint(int32_t x, int32_t y, const Color& color);

	void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const Color& color);

	void DrawTriangle(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, const Color& color);

private:
	uint32_t mWidth = 0;
	uint32_t mHeight = 0;
	std::vector<uint8_t> mColors;
};

} // namespace srend