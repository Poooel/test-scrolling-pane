#include "state/ThreadState.h"

ThreadState::ThreadState(std::function<void(ApplicationState& applicationState)> threadFunction): thread(threadFunction) {
}
