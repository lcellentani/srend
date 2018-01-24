#include "display.h"
#include "events.h"
#include "rasterizer.h"
#include "timing.h"
#include "meshloader.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <array>

#include "glm\geometric.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace srend;

int main(int, char**) {
	const uint32_t cWidth = 600;
	const uint32_t cHeight = 600;
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

			/*rasterizer->DrawPoint(10, 10, Color{ 255, 0, 0, 255 });
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
			}*/

			//rasterizer->DrawTriangle({ 10, 70 }, { 50, 160 }, { 70, 80 }, { 255, 0, 0, 255 });
			//rasterizer->DrawTriangle({ 180, 50 }, { 150, 1 }, { 70, 180 }, { 255, 255, 255, 255 });
			//rasterizer->DrawTriangle({ 180, 150 }, { 120, 160 }, { 130, 180 }, { 0, 255, 0, 255 });

			glm::vec3 lightDirection(0.0f, 0.0f, -1.0f);
			auto& shape = model->GetShape(0);
			float halfWidth = (float)cWidth * 0.5f;
			float halfHeight = (float)cHeight * 0.5f;
			std::array<glm::vec3, 3> screenCoords;
			std::array<glm::vec2, 3> texcoord0;
			std::array<glm::vec3, 3> worldCoords;
			for (std::size_t i = 0; i < shape.mFaces.size(); i++) {
				auto face = shape.mFaces[i];
				for (std::size_t n = 0; n < 3; n++) {
					auto p = face.mVertices[n];
					int x = (int)((p.mPosition.x + 1.0f) * halfWidth + 0.5f);
					int y = (int)((p.mPosition.y + 1.0f) * halfHeight + 0.5f);
					screenCoords[n] = glm::vec3(x, y, p.mPosition.z);
					worldCoords[n] = p.mPosition;
					texcoord0[n] = p.mTexcoord;
				}
				glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(worldCoords[2] - worldCoords[0]), glm::vec3(worldCoords[1] - worldCoords[0])));
				float intensity = glm::dot(normal, lightDirection);
				if (intensity > 0) {
					uint8_t u = (uint8_t)(intensity * 255.0f);
					rasterizer->DrawTriangle(screenCoords[0], screenCoords[1], screenCoords[2], texcoord0[0], texcoord0[1], texcoord0[2], { u, u, u, 255 });
				}
			}

			display->Present();
		}
	}

	return 0;
}