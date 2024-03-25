#ifndef TEST_SCROLLING_PANE_PLANESANIMATIONTHREAD_H
#define TEST_SCROLLING_PANE_PLANESANIMATIONTHREAD_H

#include "state/ApplicationState.h"

class PlanesAnimationThread {
    public:
        static void run(const std::stop_token& stoken, ApplicationState& applicationState);
};

#endif // TEST_SCROLLING_PANE_PLANESANIMATIONTHREAD_H
