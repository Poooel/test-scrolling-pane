#include "state/CanvasState.h"

CanvasState::CanvasState(ApplicationState& applicationState)
    : applicationState(applicationState),
      canvasStart(ImGui::GetCursorScreenPos()),
      canvasSize(ImGui::GetContentRegionAvail()),
      canvasEnd(canvasStart.x + canvasSize.x, canvasStart.y + canvasSize.y),
      mapOrigin(canvasStart.x + applicationState.scrollingOffset.x, canvasStart.y + applicationState.scrollingOffset.y),
      drawList(ImGui::GetWindowDrawList()) {
}
