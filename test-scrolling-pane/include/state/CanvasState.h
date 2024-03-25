#ifndef TEST_SCROLLING_PANE_CANVASSTATE_H
#define TEST_SCROLLING_PANE_CANVASSTATE_H

#include "imgui.h"
#include "state/ApplicationState.h"

struct CanvasState {
        explicit CanvasState(ApplicationState& applicationState);

        ApplicationState& applicationState;
        ImVec2 canvasStart;
        ImVec2 canvasSize;
        ImVec2 canvasEnd;
        ImVec2 mapOrigin;
        ImDrawList* drawList;
};

#endif // TEST_SCROLLING_PANE_CANVASSTATE_H
