#include "hello_imgui/hello_imgui.h"

#include <cmath>
#include <thread>

int main(int, char*[]) {
    ImVec2 shipCoordinates(0.0f, 0.0f);
    ImVec2 scrollingOffset(0.0f, 0.0f);

    ImVec4 shipColor(1.0f, 1.0f, 1.0f, 1.0f);
    float  shipAccelerationX      = 0.0f;
    float  shipAccelerationY      = 0.0f;
    int    shipAccelerationPeriod = 100;
    bool   followShip             = false;

    bool   displayGrid     = true;
    bool   displayMarkings = true;
    int    gridStep        = 64;
    ImVec4 gridColor(0.5f, 0.5f, 0.5f, 1.0f);

    bool   displayBorder = true;
    ImVec4 borderColor(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 backgroundColor(0.2f, 0.2f, 0.2f, 1.0f);

    bool enableDemoWindow  = false;
    bool enableDebugWindow = false;

    float zoomScale = 1.0f;

    std::atomic_bool stopThreadExecution(false);

    std::thread t([&]() {
        while (!stopThreadExecution) {
            shipCoordinates.x += shipAccelerationX;
            shipCoordinates.y += shipAccelerationY;
            std::this_thread::sleep_for(std::chrono::milliseconds(shipAccelerationPeriod));
        }
    });

    auto guiFunction = [&]() {
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::CollapsingHeader("Ship settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::ColorEdit3("Ship color", (float*) &shipColor);
            if (ImGui::Button("Stop ship")) {
                shipAccelerationX = 0.0f;
                shipAccelerationY = 0.0f;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset ship")) {
                shipCoordinates.x = 0.0f;
                shipCoordinates.y = 0.0f;
            }
            // TODO: Add patterns (circle, sin, ...)
            ImGui::InputFloat("Ship acceleration X", &shipAccelerationX);
            ImGui::InputFloat("Ship acceleration Y", &shipAccelerationY);
            if (ImGui::InputInt("Ship acceleration period", &shipAccelerationPeriod)) {
                if (shipAccelerationPeriod < 1) {
                    shipAccelerationPeriod = 1;
                }
            }
            ImGui::Checkbox("Follow ship", &followShip);
        }
        if (ImGui::CollapsingHeader("Grid settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Display grid", &displayGrid);
            ImGui::Checkbox("Display markings", &displayMarkings);
            ImGui::InputInt("Grid step", &gridStep);
            ImGui::ColorEdit3("Grid color", (float*) &gridColor);
        }
        if (ImGui::CollapsingHeader("Background settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Display border", &displayBorder);
            ImGui::ColorEdit3("Border color", (float*) &borderColor);
            ImGui::ColorEdit3("Background color", (float*) &backgroundColor);
        }
        if (ImGui::CollapsingHeader("Misc settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Enable Demo Window", &enableDemoWindow);
            ImGui::Checkbox("Enable Debug Window", &enableDebugWindow);
        }
        ImGui::End();

        ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
        ImVec2 canvas_sz = ImGui::GetContentRegionAvail();

        if (canvas_sz.x < 50.0f) {
            canvas_sz.x = 50.0f;
        }
        if (canvas_sz.y < 50.0f) {
            canvas_sz.y = 50.0f;
        }

        ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(canvas_p0, canvas_p1, ImColor(backgroundColor));

        if (displayBorder) {
            draw_list->AddRect(canvas_p0, canvas_p1, ImColor(borderColor));
        }

        ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft);
        ImGuiIO&     io        = ImGui::GetIO();
        const bool   isActive  = ImGui::IsItemActive();
        bool         isHovered = ImGui::IsItemHovered();
        const ImVec2 origin(canvas_p0.x + scrollingOffset.x, canvas_p0.y + scrollingOffset.y);

        if (isHovered) {
            if (io.MouseWheel != 0.0f) {
                zoomScale += io.MouseWheel * 0.1f;
                zoomScale  = zoomScale < 0.1f ? 0.1f : zoomScale;
                zoomScale  = zoomScale > 2.0f ? 2.0f : zoomScale;
            }
        }

        if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) && !followShip) {
            scrollingOffset.x += io.MouseDelta.x;
            scrollingOffset.y += io.MouseDelta.y;
        }

        if (followShip) {
            scrollingOffset.x = (canvas_sz.x / 2) - shipCoordinates.x;
            scrollingOffset.y = (canvas_sz.y / 2) - shipCoordinates.y;
        }

        draw_list->PushClipRect(canvas_p0, canvas_p1, true);

        if (displayGrid) {
            float zoomedGridStep = gridStep * zoomScale;

            // vertical lines
            for (float x = fmodf(scrollingOffset.x, zoomedGridStep); x < canvas_sz.x; x += zoomedGridStep) {
                float x0 = canvas_p0.x + x;
                float y0 = canvas_p0.y;
                float y1 = canvas_p1.y;

                draw_list->AddLine(ImVec2(x0, y0), ImVec2(x0, y1), ImColor(gridColor));

                if (displayMarkings) {
                    ImGui::SetCursorPos(ImVec2(x0 - 10, y0 + 5));
                    // TODO: Fix flickering of markings
                    // going left or right seems to change the behavior ((+1 or 2) or (-1 or 2))
                    float fakeX = (x0 - origin.x) / zoomScale;
                    ImGui::Text("%.0f", fakeX);
                }
            }

            // horizontal lines
            for (float y = fmodf(scrollingOffset.y, zoomedGridStep); y < canvas_sz.y; y += zoomedGridStep) {
                float x0 = canvas_p0.x;
                float y0 = canvas_p0.y + y;
                float x1 = canvas_p1.x;

                draw_list->AddLine(ImVec2(x0, y0), ImVec2(x1, y0), ImColor(gridColor));

                if (displayMarkings) {
                    ImGui::SetCursorPos(ImVec2(x0 + 5, y0));
                    float fakeY = (y0 - origin.y) / zoomScale;
                    ImGui::Text("%.0f", fakeY);
                }
            }
        }

        if (enableDebugWindow) {
            ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Origin X: %.0f", origin.x);
            ImGui::SameLine();
            ImGui::Text("Origin Y: %.0f", origin.y);
            ImGui::Text("Canvas P0 X: %.0f", canvas_p0.x);
            ImGui::SameLine();
            ImGui::Text("Canvas P0 Y: %.0f", canvas_p0.y);
            ImGui::Text("Canvas P1 X: %.0f", canvas_p1.x);
            ImGui::SameLine();
            ImGui::Text("Canvas P1 Y: %.0f", canvas_p1.y);
            ImGui::Text("Canvas SZ X: %.0f", canvas_sz.x);
            ImGui::SameLine();
            ImGui::Text("Canvas SZ Y: %.0f", canvas_sz.y);
            ImGui::Text("Scrolling X: %.0f", scrollingOffset.x);
            ImGui::SameLine();
            ImGui::Text("Scrolling Y: %.0f", scrollingOffset.y);
            ImGui::Text("Ship X: %.0f", shipCoordinates.x);
            ImGui::SameLine();
            ImGui::Text("Ship Y: %.0f", shipCoordinates.y);
            ImGui::Text("Zoom Scale %.1f", zoomScale);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Text("Mouse Pos X: %.0f", io.MousePos.x);
            ImGui::SameLine();
            ImGui::Text("Mouse Pos Y: %.0f", io.MousePos.y);
            ImGui::Text("Actual Ship X: %.0f", shipCoordinates.x + origin.x);
            ImGui::SameLine();
            ImGui::Text("Actual Ship Y: %.0f", shipCoordinates.y + origin.y);
            ImGui::End();
        }

        if (enableDemoWindow) {
            ImGui::ShowDemoWindow();
        }

        draw_list->AddCircleFilled(
            ImVec2(shipCoordinates.x + origin.x, shipCoordinates.y + origin.y), 10 * zoomScale, ImColor(shipColor)
        );

        draw_list->PopClipRect();
    };

    const std::string            windowTitle             = "Test Scrolling Pane";
    const HelloImGui::ScreenSize screenSize              = { 1280, 720 };
    const bool                   autoSizeWindow          = false;
    const bool                   restorePreviousGeometry = false;
    const float                  fpsIdle                 = 0.0f;

    HelloImGui::Run(guiFunction, windowTitle, autoSizeWindow, restorePreviousGeometry, screenSize, fpsIdle);

    stopThreadExecution = true;
    t.join();

    return 0;
}
