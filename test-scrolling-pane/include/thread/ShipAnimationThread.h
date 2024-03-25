#ifndef TEST_SCROLLING_PANE_SHIPANIMATIONTHREAD_H
#define TEST_SCROLLING_PANE_SHIPANIMATIONTHREAD_H

#include "state/ApplicationState.h"

class ShipAnimationThread {
    public:
        static void run(const std::stop_token& stoken, ApplicationState& applicationState);
};

#endif // TEST_SCROLLING_PANE_SHIPANIMATIONTHREAD_H
