#include "display.h"
#include "events.h"
#include "rasterizer.h"
#include "timing.h"

#include <memory>
#include <iostream>

using namespace srend;

int main(int, char**) {
	const uint32_t cWidth = 400;
	const uint32_t cHeight = 300;
	std::unique_ptr<Display> display = std::make_unique<Display>(cWidth, cHeight, "srend");
	if (display->IsValid()) {
		std::unique_ptr<EventsHandler> eventsHandler = std::make_unique<EventsHandler>();
		std::unique_ptr<Rasterizer> rasterizer = std::make_unique<Rasterizer>();
		rasterizer->SetViewport(cWidth, cHeight);
		
		display->SetFramebufferUpdater([&rasterizer](uint32_t /*width*/, uint32_t /*height*/, uint8_t /*depth*/, std::vector<uint8_t>& brga) {
			brga = rasterizer->GetColorBuffer();
		});

		int64_t lastTime = GetTime();
		
		bool requestExit = false;
		while (!requestExit) {
			std::unique_ptr<Event> event = eventsHandler->Pool();
			switch (event->mType) {
			case Event::Exit:
				requestExit = true;
			}
		
			int64_t now = GetTime();
			float delta = ((float)(now - lastTime) / (float)GetFrequency()) * 1000.0f;
			std::cout << delta << std::endl;
			lastTime = now;

			rasterizer->DrawPoint(10, 10, Color{ 255, 0, 0, 255 });

			rasterizer->DrawLine(13, 20, 80, 40, Color{ 255, 255, 255, 255 });
			rasterizer->DrawLine(20, 13, 40, 80, Color{ 255, 0, 0, 255 });
			rasterizer->DrawLine(80, 40, 13, 20, Color{ 255, 0, 0, 255 });

			display->Present();
		}
	}

	return 0;
}