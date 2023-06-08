#include "EventEmitter.h"

namespace dubu::event {

void EventEmitter::RegisterListener(dubu::util::IdType eventId,
                                    Listener           listener) {
	if (mListeners.find(eventId) == mListeners.end()) {
		mListeners.emplace(eventId, std::vector<Listener>());
	}

	mListeners[eventId].push_back(listener);
}

}  // namespace dubu::event