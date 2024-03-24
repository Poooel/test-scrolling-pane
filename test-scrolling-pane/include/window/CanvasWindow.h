#ifndef TEST_SCROLLING_PANE_CANVASWINDOW_H
#define TEST_SCROLLING_PANE_CANVASWINDOW_H

#include "state/ApplicationState.h"

class CanvasWindow {
    public:
        static void gui(ApplicationState& applicationState);

    private:
        static void drawGrid(ApplicationState& applicationState);
        static void drawShip(ApplicationState& applicationState);
        static void drawPlanes(ApplicationState& applicationState);
};

#endif // TEST_SCROLLING_PANE_CANVASWINDOW_H
