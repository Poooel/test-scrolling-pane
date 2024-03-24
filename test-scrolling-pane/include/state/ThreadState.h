#ifndef TEST_SCROLLING_PANE_THREADSTATE_H
#define TEST_SCROLLING_PANE_THREADSTATE_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include "ApplicationState.h"

struct ThreadState {
        ThreadState(std::function<void(ApplicationState& applicationState)> threadFunction);

        bool                    startExecution = false;
        std::mutex              mutex;
        std::condition_variable conditionVariable;
        std::jthread            thread;
};

#endif // TEST_SCROLLING_PANE_THREADSTATE_H
