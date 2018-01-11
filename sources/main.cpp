#include "display.h"
#include "events.h"
#include <memory>

#include "SDL.h"

void updater(uint32_t width, uint32_t height, uint8_t depth, std::vector<uint8_t>& brga) {
	uint32_t size = width * height * depth;
	for (uint32_t n = 0; n < size; n += depth) {
		brga[n] = 0;
		brga[n + 1] = 0;
		brga[n + 2] = 255;
		brga[n + 3] = 255;
	}
}

int main(int, char**) {
	std::unique_ptr<Display> display = std::make_unique<Display>(800, 600, "srend");
	if (display->IsValid()) {
		display->SetFramebufferUpdater(updater);

		SDL_Event e;
		bool requestExit = false;
		while (!requestExit) {
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) {
					requestExit = true;
				}
			}

			display->Present();
		}
	}

	return 0;
}