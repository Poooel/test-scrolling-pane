#include "window/DebugWindow.h"

void DebugWindow::gui(ApplicationState& applicationState) {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Scrolling X: %.0f", applicationState.scrollingOffset.x);
    ImGui::SameLine();
    ImGui::Text("Scrolling Y: %.0f", applicationState.scrollingOffset.y);
    ImGui::Text("Ship X: %.0f", applicationState.shipCoordinates.x);
    ImGui::SameLine();
    ImGui::Text("Ship Y: %.0f", applicationState.shipCoordinates.y);
    ImGui::Text("Zoom Scale %.1f", applicationState.zoomScale);
    ImGui::Text("Mouse Pos X: %.0f", io.MousePos.x);
    ImGui::SameLine();
    ImGui::Text("Mouse Pos Y: %.0f", io.MousePos.y);
    ImGui::Text("Mouse Delta X: %.0f", io.MouseDelta.x);
    ImGui::SameLine();
    ImGui::Text("Mouse Delta Y: %.0f", io.MouseDelta.y);
    ImGui::Text("Time: %.3f", ImGui::GetTime());
    ImGui::Text("Trail size: %zu", applicationState.trail.size());

    ImGui::End();
}
