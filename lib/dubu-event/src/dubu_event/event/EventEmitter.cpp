#include "EventEmitter.h"

namespace dubu::event {

void EventEmitter::RegisterListener(dubu::util::IdType eventId, Listener listener) {
  mListeners[eventId].push_back(listener);
}

}  // namespace dubu::event