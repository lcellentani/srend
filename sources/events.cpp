#include "events.h"
#include "SDL.h"

namespace srend
{

EventsHandler::EventsHandler() {}

EventsHandler::~EventsHandler() {}

std::unique_ptr<Event> EventsHandler::Pool() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		uint32_t type = event.type;
		if (type == SDL_QUIT || (type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
			return std::make_unique<ExitEvent>();
		}
	}
	return std::make_unique<Event>();
}

} // namespace srend