#pragma once

#include <fstream>
#include <cstdint>

#pragma pack(push,1)
struct TGA_Header
{
	uint8_t idlength;
	uint8_t colormaptype;
	uint8_t datatypecode;
	uint16_t colormaporigin;
	uint16_t colormaplength;
	uint8_t colormapdepth;
	uint16_t x_origin;
	uint16_t y_origin;
	uint16_t width;
	uint16_t height;
	uint8_t bitsperpixel;
	uint8_t imagedescriptor;
};
#pragma pack(pop)

struct TGAColor
{
	uint8_t bgra[4];
	uint8_t bytespp;

	TGAColor() : bgra(), bytespp(1) {
		memset(bgra, 0, sizeof(uint8_t) * 4);
	}

	TGAColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255) : bgra(), bytespp(4) {
		bgra[0] = B;
		bgra[1] = G;
		bgra[2] = R;
		bgra[3] = A;
	}

	TGAColor(uint8_t v) : bgra(), bytespp(1) {
		memset(bgra, 0, sizeof(uint8_t) * 4);
		bgra[0] = v;
	}

	TGAColor(const uint8_t *p, uint8_t bpp) : bgra(), bytespp(bpp) {
		memset(bgra, 0, sizeof(uint8_t) * 4);
		for (uint8_t i = 0; i < bpp; i++) {
			bgra[i] = p[i];
		}
	}

	uint8_t& operator[](const uint8_t i) { return bgra[i]; }

	TGAColor operator *(float intensity) const {
		TGAColor res = *this;
		intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
		for (uint8_t i = 0; i < 4; i++) res.bgra[i] = static_cast<uint8_t>(bgra[i] * intensity);
		return res;
	}
};

class TGAImage
{
public:
	enum Format {
		GRAYSCALE = 1, RGB = 3, RGBA = 4
	};

	TGAImage();
	TGAImage(uint16_t width, uint16_t height, uint8_t bpp);
	TGAImage(const TGAImage& img);

	~TGAImage();

	TGAImage& operator= (const TGAImage& img);

	bool Read(const char* filename);
	bool Write(const char* filename, bool compressed = true);

	void Clear();

	bool FlipHorizontally();
	bool FlipVertically();

	bool Scale(uint16_t width, uint16_t height);

	uint16_t GetWidth() const {
		return mWidth;
	}

	uint16_t GetHeight() const {
		mHeight;
	}

	uint8_t GetBitPerPixels() const {
		mBitPerPixels;
	}

	uint8_t* const GetData() const;

	TGAColor GetPixel(uint16_t x, uint16_t y) const;
	bool SetPixel(uint16_t x, uint16_t y, TGAColor& c);
	bool SetPixel(uint16_t x, uint16_t y, const TGAColor& c);

private:
	bool LoadDataRLE(std::ifstream& in);
	bool UnloadDataRLE(std::ofstream& out);

private:
	uint8_t* mData;
	uint16_t mWidth;
	uint16_t mHeight;
	uint8_t mBitPerPixels;
};
