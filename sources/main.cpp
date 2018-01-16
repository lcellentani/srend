#include "display.h"
#include "events.h"
#include "rasterizer.h"
#include "timing.h"
#include "meshloader.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <iostream>

using namespace srend;

int main(int, char**) {
	const uint32_t cWidth = 200;
	const uint32_t cHeight = 200;
	std::unique_ptr<Display> display = std::make_unique<Display>(cWidth, cHeight, "srend");
	if (display->IsValid()) {
		std::unique_ptr<Model> model = std::make_unique<Model>();
		model->Load("african_head/african_head.obj", true);
		if (!model->IsValid()) {
			return 0;
		}

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
			rasterizer->DrawLine({ 13.f, 20.f }, { 80.f, 40.f }, Color{ 255, 255, 255, 255 });
			rasterizer->DrawLine({ 20.f, 13.f }, { 40.f, 80.f }, Color{ 255, 0, 0, 255 });
			rasterizer->DrawLine({ 80.f, 40.f }, { 13.f, 20.f }, Color{ 255, 0, 0, 255 });
			
			auto& shape = model->GetShape(0);
			float halfWidth = (float)cWidth / 2.0f;
			float halfHeight = (float)cHeight / 2.0f;
			for (std::size_t i = 0; i < shape.mFaces.size(); i++) {
				auto face = shape.mFaces[i];
				for (std::size_t n = 0; n < 3; n++) {
					auto pt0 = face.mVertices[n];
					auto pt1 = face.mVertices[(n + 1) % 3];
					glm::vec2 p0((pt0.mPosition.x + 1.0f) * halfWidth, (pt0.mPosition.y + 1.0f) * halfHeight);
					glm::vec2 p1((pt1.mPosition.x + 1.0f) * halfWidth, (pt1.mPosition.y + 1.0f) * halfHeight);
					rasterizer->DrawLine(p0, p1, Color{ 255, 255, 255, 255 });
				}
			}

			rasterizer->DrawTriangle({ 10, 70 }, { 50, 160 }, { 70, 80 }, { 255, 0, 0, 255 });
			rasterizer->DrawTriangle(glm::ivec2(180, 50), glm::ivec2(150, 1), glm::ivec2(70, 180), { 255, 255, 255, 255 });
			rasterizer->DrawTriangle(glm::ivec2(180, 150), glm::ivec2(120, 160), glm::ivec2(130, 180), { 0, 255, 0, 255 });

			display->Present();
		}
	}

	return 0;
}