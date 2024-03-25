#ifndef TEST_SCROLLING_PANE_CANVASWINDOW_H
#define TEST_SCROLLING_PANE_CANVASWINDOW_H

#include "state/ApplicationState.h"
#include "state/CanvasState.h"

class CanvasWindow {
    public:
        static void gui(ApplicationState& applicationState);

    private:
        static void drawGrid(CanvasState& canvasState);
        static void drawShip(CanvasState& canvasState);
        static void drawPlanes(CanvasState& canvasState);
};

#endif // TEST_SCROLLING_PANE_CANVASWINDOW_H
