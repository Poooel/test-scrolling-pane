#include "hello_imgui/hello_imgui.h"

#include <cmath>
#include <thread>

int main(int, char*[]) {
    ImVec2 pointOrigin(321.0f, 448.0f);
    ImVec2 scrolling(0.0f, 0.0f);

    bool enableGrid               = true;
    bool enableDebug              = false;
    bool enableAccelerationWindow = false;

    float zoomScale = 1.0f;

    bool  enableAcceleration = false;
    float xAcceleration      = 0.0f;
    float yAcceleration      = 0.0f;
    int   accelerationPeriod = 100;

    const float GRID_STEP   = 64.0f;
    const ImU32 GRID_COLOR  = IM_COL32(200, 200, 200, 40);
    const ImU32 POINT_COLOR = IM_COL32(255, 255, 0, 255);

    std::atomic_bool stopThreadExecution(false);

    auto threadFunction = [&]() {
        while (!stopThreadExecution) {
            pointOrigin.x += xAcceleration;
            pointOrigin.y += yAcceleration;
            std::this_thread::sleep_for(std::chrono::milliseconds(accelerationPeriod));
        }
    };

    auto startThread = [&](std::thread& t) {
        stopThreadExecution = false;
        t                   = std::thread(threadFunction);
    };

    auto stopThread = [&](std::thread& t) {
        stopThreadExecution = true;
        if (t.joinable()) {
            t.join();
        }
    };

    std::thread t;

    auto guiFunction = [&]() {
        ImGui::Checkbox("Enable grid", &enableGrid);
        ImGui::Checkbox("Enable debug", &enableDebug);
        ImGui::Checkbox("Enable Acceleration Window", &enableAccelerationWindow);

        if (ImGui::Button("Reset zoom scale")) {
            zoomScale = 1.0f;
        }

        if (enableAccelerationWindow) {
            ImGui::Begin("Acceleration");
            if (ImGui::Button("Enable acceleration")) {
                enableAcceleration = !enableAcceleration;
                if (enableAcceleration) {
                    startThread(t);
                } else {
                    stopThread(t);
                }
            }
            ImGui::SliderFloat("X acceleration", &xAcceleration, -10.0f, 10.0f);
            ImGui::SliderFloat("Y acceleration", &yAcceleration, -10.0f, 10.0f);
            ImGui::SliderInt("Acceleration period", &accelerationPeriod, 1, 10000);
            ImGui::End();
        }

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
        draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
        draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

        ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft);
        ImGuiIO&     io        = ImGui::GetIO();
        const bool   is_active = ImGui::IsItemActive();
        const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y);
        bool         isHovered = ImGui::IsItemHovered();

        if (isHovered) {
            float scroll = io.MouseWheel;

            if (scroll != 0.0f) {
                zoomScale += scroll * 0.1f;
                zoomScale  = zoomScale < 0.1f ? 0.1f : zoomScale;
                zoomScale  = zoomScale > 2.0f ? 2.0f : zoomScale;
            }
        }

        const float mouse_threshold_for_pan = 0.0f;
        if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left, mouse_threshold_for_pan)) {
            scrolling.x += io.MouseDelta.x;
            scrolling.y += io.MouseDelta.y;
        }

        draw_list->PushClipRect(canvas_p0, canvas_p1, true);

        if (enableGrid) {
            // vertical lines
            float zoomedGridStep = GRID_STEP * zoomScale;

            for (float x = fmodf(scrolling.x, zoomedGridStep); x < canvas_sz.x; x += zoomedGridStep) {
                float x0 = canvas_p0.x + x;
                float y0 = canvas_p0.y;
                float y1 = canvas_p1.y;

                draw_list->AddLine(ImVec2(x0, y0), ImVec2(x0, y1), GRID_COLOR);

                ImGui::SetCursorPos(ImVec2(x0 - 10, y0 + 5));
                float fakeX = std::round((x0 - origin.x) / zoomScale);
                ImGui::Text("%.0f", fakeX);
            }

            // horizontal lines
            for (float y = fmodf(scrolling.y, zoomedGridStep); y < canvas_sz.y; y += zoomedGridStep) {
                float x0 = canvas_p0.x;
                float y0 = canvas_p0.y + y;
                float x1 = canvas_p1.x;

                draw_list->AddLine(ImVec2(x0, y0), ImVec2(x1, y0), GRID_COLOR);

                ImGui::SetCursorPos(ImVec2(x0 + 5, y0));
                float fakeY = std::round((y0 - origin.y) / zoomScale);
                ImGui::Text("%.0f", fakeY);
            }
        }

        ImVec2 mousePos = io.MousePos;

        if (enableDebug) {
            ImGui::Begin("Debug");
            ImGui::Text("Origin X: %f", origin.x);
            ImGui::SameLine();
            ImGui::Text("Origin Y: %f", origin.y);
            ImGui::Text("Canvas P0 X: %f", canvas_p0.x);
            ImGui::SameLine();
            ImGui::Text("Canvas P0 Y: %f", canvas_p0.y);
            ImGui::Text("Canvas P1 X: %f", canvas_p1.x);
            ImGui::SameLine();
            ImGui::Text("Canvas P1 Y: %f", canvas_p1.y);
            ImGui::Text("Canvas SZ X: %f", canvas_sz.x);
            ImGui::SameLine();
            ImGui::Text("Canvas SZ Y: %f", canvas_sz.y);
            ImGui::Text("Scrolling X: %f", scrolling.x);
            ImGui::SameLine();
            ImGui::Text("Scrolling Y: %f", scrolling.y);
            ImGui::Text("Point X: %f", pointOrigin.x);
            ImGui::SameLine();
            ImGui::Text("Point Y: %f", pointOrigin.y);
            ImGui::Text("Zoom Scale %f", zoomScale);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Text("Mouse Pos X: %f", mousePos.x);
            ImGui::SameLine();
            ImGui::Text("Mouse Pos Y: %f", mousePos.y);
            ImGui::End();
        }

        draw_list->AddCircleFilled(
            ImVec2(pointOrigin.x + origin.x, pointOrigin.y + origin.y), 10 * zoomScale, POINT_COLOR
        );

        draw_list->PopClipRect();
    };

    // TODO: Zoom is inverted
    // When you zoom in, it should move faster and zoom out it should move slower (currently the opposite)
    // TODO: Find a way to have a button to center on the point
    // There must be a way to find the correct scrolling offset based on the point coords and some other thing

    const std::string            windowTitle             = "Test Scrolling Pane";
    const HelloImGui::ScreenSize screenSize              = { 1280, 720 };
    const bool                   autoSizeWindow          = false;
    const bool                   restorePreviousGeometry = true;

    HelloImGui::Run(guiFunction, windowTitle, autoSizeWindow, restorePreviousGeometry, screenSize);

    stopThread(t);

    return 0;
}
