#pragma once

#include <memory>

class Event {
public:
	enum Type {
		Null,
		Exit,
		Key,
		Mouse,
		Size,
		Char
	};

	Event() : mType(Type::Null) {}
	Event(Type type) : mType(type) {}
	virtual ~Event() = default;

	Type mType;
};

class ExitEvent : public Event {
public:
	ExitEvent() : Event(Event::Exit) {}
};

class EventsHandler final {
public:
	EventsHandler();
	~EventsHandler();

	std::unique_ptr<Event> Pool();
};