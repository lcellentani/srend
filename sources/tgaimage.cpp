#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <ctime>
#include <cmath>

#include "tgaimage.h"

TGAImage::TGAImage() : mData(nullptr), mWidth(0), mHeight(0), mBitPerPixels(0) {}

TGAImage::TGAImage(uint16_t width, uint16_t height, uint8_t bpp) : mData(nullptr), mWidth(width), mHeight(height), mBitPerPixels(bpp)
{
	uint32_t nbytes = mWidth * mHeight * mBitPerPixels;
	mData = new uint8_t[nbytes];
	memset(mData, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage& img) : mData(nullptr), mWidth(img.mWidth), mHeight(img.mHeight), mBitPerPixels(img.mBitPerPixels)
{
	uint32_t nbytes = mWidth * mHeight * mBitPerPixels;
	mData = new uint8_t[nbytes];
	memcpy(mData, img.mData, nbytes);
}

TGAImage::~TGAImage()
{
	delete[] mData;
	mData = nullptr;
}

TGAImage & TGAImage::operator =(const TGAImage &img)
{
	if (this != &img) {
		if (mData)
			delete[] mData;

		mWidth = img.mWidth;
		mHeight = img.mHeight;
		mBitPerPixels = img.mBitPerPixels;
		uint32_t nbytes = mWidth * mHeight * mBitPerPixels;
		mData = new uint8_t[nbytes];
		memcpy(mData, img.mData, nbytes);
	}
	return *this;
}

bool TGAImage::Read(const char* filename)
{
	if (!filename) {
		return false;
	}

	if (mData)
		delete[] mData;
	mData = nullptr;

	std::ifstream in;
	in.open(filename, std::ios::binary);
	if (!in.is_open()) {
		std::cerr << "can't open file " << filename << "\n";
		in.close();
		return false;
	}
	TGA_Header header;
	in.read((char *)&header, sizeof(header));
	if (!in.good()) {
		in.close();
		std::cerr << "an error occured while reading the header\n";
		return false;
	}
	mWidth = header.width;
	mHeight = header.height;
	mBitPerPixels = header.bitsperpixel >> 3;
	if (mWidth == 0 || mHeight == 0 || (mBitPerPixels != GRAYSCALE && mBitPerPixels != RGB && mBitPerPixels != RGBA)) {
		in.close();
		std::cerr << "bad bpp (or width/height) value\n";
		return false;
	}
	uint32_t nbytes = mWidth * mHeight * mBitPerPixels;
	mData = new uint8_t[nbytes];
	if (3 == header.datatypecode || 2 == header.datatypecode) {
		in.read((char *)mData, nbytes);
		if (!in.good()) {
			in.close();
			std::cerr << "an error occured while reading the data\n";
			return false;
		}
	}
	else if (10 == header.datatypecode || 11 == header.datatypecode) {
		if (!LoadDataRLE(in)) {
			in.close();
			std::cerr << "an error occured while reading the data\n";
			return false;
		}
	}
	else {
		in.close();
		std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
		return false;
	}
	if (!(header.imagedescriptor & 0x20)) {
		FlipVertically();
	}
	if (header.imagedescriptor & 0x10) {
		FlipHorizontally();
	}
	std::cerr << mWidth << "x" << mHeight << "/" << mBitPerPixels * 8 << "\n";
	in.close();
	return true;
}

bool TGAImage::LoadDataRLE(std::ifstream& in)
{
	uint32_t pixelcount = mWidth * mHeight;
	uint32_t currentpixel = 0;
	uint32_t currentbyte = 0;
	TGAColor colorbuffer;
	do {
		uint8_t chunkheader = 0;
		chunkheader = in.get();
		if (!in.good()) {
			std::cerr << "an error occured while reading the data\n";
			return false;
		}
		if (chunkheader < 128) {
			chunkheader++;
			for (uint8_t i = 0; i < chunkheader; i++) {
				in.read((char *)colorbuffer.bgra, mBitPerPixels);
				if (!in.good()) {
					std::cerr << "an error occured while reading the header\n";
					return false;
				}
				for (uint8_t t = 0; t < mBitPerPixels; t++) {
					mData[currentbyte++] = colorbuffer.bgra[t];
				}
				currentpixel++;
				if (currentpixel>pixelcount) {
					std::cerr << "Too many pixels read\n";
					return false;
				}
			}
		}
		else {
			chunkheader -= 127;
			in.read((char *)colorbuffer.bgra, mBitPerPixels);
			if (!in.good()) {
				std::cerr << "an error occured while reading the header\n";
				return false;
			}
			for (uint8_t i = 0; i < chunkheader; i++) {
				for (uint8_t t = 0; t < mBitPerPixels; t++) {
					mData[currentbyte++] = colorbuffer.bgra[t];
				}
				currentpixel++;
				if (currentpixel>pixelcount) {
					std::cerr << "Too many pixels read\n";
					return false;
				}
			}
		}
	} while (currentpixel < pixelcount);
	return true;
}

bool TGAImage::Write(const char *filename, bool compressed)
{
	if (!filename) {
		return false;
	}

	uint8_t developer_area_ref[4] = { 0, 0, 0, 0 };
	uint8_t extension_area_ref[4] = { 0, 0, 0, 0 };
	uint8_t footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };
	std::ofstream out;
	out.open(filename, std::ios::binary);
	if (!out.is_open()) {
		std::cerr << "can't open file " << filename << "\n";
		out.close();
		return false;
	}
	TGA_Header header;
	memset((void *)&header, 0, sizeof(header));
	header.bitsperpixel = mBitPerPixels << 3;
	header.width = mWidth;
	header.height = mHeight;
	header.datatypecode = (mBitPerPixels == GRAYSCALE ? (compressed ? 11 : 3) : (compressed ? 10 : 2));
	header.imagedescriptor = 0x20; // top-left origin
	out.write((char *)&header, sizeof(header));
	if (!out.good()) {
		out.close();
		std::cerr << "can't dump the tga file\n";
		return false;
	}
	if (!compressed) {
		out.write((char *)mData, mWidth * mHeight * mBitPerPixels);
		if (!out.good()) {
			std::cerr << "can't unload raw data\n";
			out.close();
			return false;
		}
	}
	else {
		if (!UnloadDataRLE(out)) {
			out.close();
			std::cerr << "can't unload rle data\n";
			return false;
		}
	}
	out.write((char *)developer_area_ref, sizeof(developer_area_ref));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.write((char *)extension_area_ref, sizeof(extension_area_ref));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.write((char *)footer, sizeof(footer));
	if (!out.good()) {
		std::cerr << "can't dump the tga file\n";
		out.close();
		return false;
	}
	out.close();
	return true;
}

// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
bool TGAImage::UnloadDataRLE(std::ofstream& out)
{
	const uint8_t max_chunk_length = 128;
	uint32_t npixels = mWidth * mHeight;
	uint32_t curpix = 0;
	while (curpix<npixels) {
		uint32_t chunkstart = curpix * mBitPerPixels;
		uint32_t curbyte = curpix *mBitPerPixels;
		uint8_t run_length = 1;
		bool raw = true;
		while (curpix + run_length < npixels && run_length < max_chunk_length) {
			bool succ_eq = true;
			for (uint8_t t = 0; succ_eq && t < mBitPerPixels; t++) {
				succ_eq = (mData[curbyte + t] == mData[curbyte + t + mBitPerPixels]);
			}
			curbyte += mBitPerPixels;
			if (1 == run_length) {
				raw = !succ_eq;
			}
			if (raw && succ_eq) {
				run_length--;
				break;
			}
			if (!raw && !succ_eq) {
				break;
			}
			run_length++;
		}
		curpix += run_length;
		out.put(raw ? run_length - 1 : run_length + 127);
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		out.write((char *)(mData + chunkstart), (raw ? run_length * mBitPerPixels : mBitPerPixels));
		if (!out.good()) {
			std::cerr << "can't dump the tga file\n";
			return false;
		}
	}
	return true;
}

void TGAImage::Clear()
{
	if (mData) {
		assert(mWidth != 0 && mHeight != 0 && mBitPerPixels != 0);
		memset((void *)mData, 0, mWidth * mHeight * mBitPerPixels);
	}
}

bool TGAImage::FlipHorizontally()
{
	if (!mData) {
		return false;
	}
	uint16_t half = mWidth >> 1;
	for (uint16_t i = 0; i < half; i++) {
		for (uint16_t j = 0; j < mHeight; j++) {
			TGAColor c1 = GetPixel(i, j);
			TGAColor c2 = GetPixel(mWidth - 1 - i, j);
			SetPixel(i, j, c2);
			SetPixel(mWidth - 1 - i, j, c1);
		}
	}
	return true;
}

bool TGAImage::FlipVertically()
{
	if (!mData) {
		return false;
	}
	uint32_t bytes_per_line = mWidth * mBitPerPixels;
	uint8_t* line = new uint8_t[bytes_per_line];
	uint16_t half = mHeight >> 1;
	for (uint16_t j = 0; j < half; j++) {
		uint32_t l1 = j * bytes_per_line;
		uint32_t l2 = (mHeight - 1 - j) * bytes_per_line;
		memmove((void *)line, (void *)(mData + l1), bytes_per_line);
		memmove((void *)(mData + l1), (void *)(mData + l2), bytes_per_line);
		memmove((void *)(mData + l2), (void *)line, bytes_per_line);
	}
	delete[] line;
	return true;
}

bool TGAImage::Scale(uint16_t width, uint16_t height)
{
	if (width == 0 || height == 0 || !mData) {
		return false;
	}
	uint8_t* data = new uint8_t[width * height * mBitPerPixels];
	uint32_t nscanline = 0;
	uint32_t oscanline = 0;
	uint16_t erry = 0;
	uint32_t nlinebytes = width * mBitPerPixels;
	uint32_t olinebytes = mWidth * mBitPerPixels;
	for (uint16_t j = 0; j < mHeight; j++) {
		int errx = mWidth - width;
		int nx = -mBitPerPixels;
		int ox = -mBitPerPixels;
		for (uint16_t i = 0; i < mWidth; i++) {
			ox += mBitPerPixels;
			errx += width;
			while (errx >= mWidth) {
				errx -= mWidth;
				nx += mBitPerPixels;
				memcpy(data + nscanline + nx, mData + oscanline + ox, mBitPerPixels);
			}
		}
		erry += height;
		oscanline += olinebytes;
		while (erry >= mHeight) {
			if (erry >= mHeight << 1) // it means we jump over a scanline
				memcpy(data + nscanline + nlinebytes, data + nscanline, nlinebytes);
			erry -= mHeight;
			nscanline += nlinebytes;
		}
	}
	delete[] mData;
	mData = data;
	mWidth = width;
	mHeight = height;
	return true;
}

uint8_t* const TGAImage::GetData() const
{
	return mData;
}

TGAColor TGAImage::GetPixel(uint16_t x, uint16_t y) const
{
	if (!mData || x >= mWidth || y >= mHeight) {
		return TGAColor();
	}
	return TGAColor(mData + (x + y * mWidth) * mBitPerPixels, mBitPerPixels);
}

bool TGAImage::SetPixel(uint16_t x, uint16_t y, TGAColor& c)
{
#if _DEBUG
	if (!mData || x >= mWidth || y >= mHeight) {
		return false;
	}
#endif
	memcpy(mData + (x + y * mWidth) * mBitPerPixels, c.bgra, mBitPerPixels);
	return true;
}

bool TGAImage::SetPixel(uint16_t x, uint16_t y, const TGAColor& c)
{
#if _DEBUG
	if (!mData || x >= mWidth || y >= mHeight) {
		return false;
	}
#endif
	memcpy(mData + (x + y * mWidth) * mBitPerPixels, c.bgra, mBitPerPixels);
	return true;
}


