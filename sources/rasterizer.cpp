#include "rasterizer.h"
#include "tgaimage.h"

#include <cassert>

namespace srend
{

struct Rasterizer::Impl
{
	TGAImage* image = nullptr;
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
	mImpl->width = width;
	mImpl->height = height;
}

void Rasterizer::SetTexel(uint16_t x, uint16_t y)
{
	assert(mImpl->image);

	mImpl->image->SetPixel(x, y, TGAColor(255, 255, 255, 255));
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