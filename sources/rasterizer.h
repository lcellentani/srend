#pragma once

#include <memory>
#include <cstdint>

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

	void SetTexel(uint16_t x, uint16_t y);

	bool Save(const char* filename);

private:
	struct Impl;
	std::unique_ptr<Impl> mImpl;
};

} // srend