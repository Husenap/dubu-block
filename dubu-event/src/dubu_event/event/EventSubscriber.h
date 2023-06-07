#pragma once

#include <functional>
#include <vector>

#include "EventEmitter.h"
#include "Token.h"

namespace dubu::event {

class EventSubscriber {
public:
	EventSubscriber() = default;
	virtual ~EventSubscriber() {}
	EventSubscriber(const EventSubscriber&) = delete;

protected:
	template <typename EventType, typename EmitterType>
	void Subscribe(std::function<void(const EventType&)> callback,
	               EmitterType&                          emitter) {
		tokens.push_back(
		    emitter.template RegisterListener<EventType>(callback));
	}

private:
	std::vector<dubu::event::Token> tokens;
};

}  // namespace dubu::event
