#include "display.h"
#include "SDL.h"

#include <iostream>

struct Display::Impl {
	Impl(uint32_t width, uint32_t height, const char* caption) : mWidth(width), mHeight(height) {
		if (SDL_Init(SDL_INIT_VIDEO) == 0) {
			mWindow = SDL_CreateWindow(caption, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mWidth, mHeight, SDL_WINDOW_SHOWN);
			if (mWindow) {
				mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
				if (mRenderer) {
					SDL_RendererInfo info;
					SDL_GetRendererInfo(mRenderer, &info);
					std::cout << "Renderer name: " << info.name << std::endl;
					std::cout << "Texture formats:" << std::endl;
					for (uint32_t i = 0; i < info.num_texture_formats; i++) {
						std::cout << "    " << SDL_GetPixelFormatName(info.texture_formats[i]) << std::endl;
					}

					SDL_SetRenderDrawColor(mRenderer, 80, 80, 80, SDL_ALPHA_OPAQUE);

					mFrameBuffer = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, mWidth, mHeight);
					mColorData.resize(mWidth * mHeight * 4);

					mInitialized = true;
				}
			}
		}
	}

	~Impl() {
		SDL_DestroyTexture(mFrameBuffer);
		mFrameBuffer = nullptr;

		SDL_DestroyRenderer(mRenderer);
		mRenderer = nullptr;

		SDL_DestroyWindow(mWindow);
		mWindow = nullptr;

		SDL_Quit();
	}

	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;
	SDL_Texture* mFrameBuffer = nullptr;

	uint32_t mWidth = 1024;
	uint32_t mHeight = 720;
	bool mInitialized = false;

	std::vector<uint8_t> mColorData;
	std::function<void(uint32_t width, uint32_t height, uint8_t depth, std::vector<uint8_t>&)> mUpdaterFunc;
};

Display::Display(uint32_t width, uint32_t height, const char* caption) : mImpl(new Impl(width, height, caption)) {
}

Display::~Display() {
	delete mImpl;
	mImpl = nullptr;
}

bool Display::IsValid() const {
	return mImpl ? mImpl->mInitialized : false;
}

void Display::Present() {
	if (!mImpl->mInitialized) { return; }

	SDL_RenderClear(mImpl->mRenderer);

	if (mImpl->mUpdaterFunc) {
		mImpl->mUpdaterFunc(mImpl->mWidth, mImpl->mHeight, 4, mImpl->mColorData);
		SDL_UpdateTexture(mImpl->mFrameBuffer, nullptr, mImpl->mColorData.data(), mImpl->mWidth * 4);
		SDL_RenderCopy(mImpl->mRenderer, mImpl->mFrameBuffer, nullptr, nullptr);
	}

	SDL_RenderPresent(mImpl->mRenderer);
}

void Display::SetFramebufferUpdater(std::function<void(uint32_t width, uint32_t height, uint8_t depth, std::vector<uint8_t>& brga)> func) {
	if (!mImpl->mInitialized) { return; }
	mImpl->mUpdaterFunc = func;
}