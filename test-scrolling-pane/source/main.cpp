#include "hello_imgui/hello_imgui.h"

#include <cmath>
#include <list>
#include <thread>

int main(int, char*[]) {
    // TODO: add speed vector north + east
    // TODO: add planes + animate planes
    // TODO: convert from x/y to lat/long
    // TODO: rework drawing + scrolling + origin (?)
    // TODO: have own timers to avoid jumps
    // TODO: ship's trail when following ship doesn't work
    // TODO: do not store trail when scrolling
    // TODO: disabling trail when following ship make ship disappears
    // TODO: adding more than one track seg fault
    // TODO: seg fault on stop
    // TODO: add offset to each plane so they don't overlap
    // TODO: add more animation for planes

    ImVec2 shipCoordinates(0.0f, 0.0f);
    ImVec2 scrollingOffset(0.0f, 0.0f);

    ImVec4            shipColor(1.0f, 1.0f, 1.0f, 1.0f);
    float             shipLinearSpeedX       = 0.0f;
    float             shipLinearSpeedY       = 0.0f;
    int               shipAccelerationPeriod = 10;
    bool              followShip             = true;
    const char*       animations[]           = { "Linear", "Sin", "Circle" };
    int               animationSelectedIndex = 0;
    float             shipSinAmplitude       = 100.0f;
    float             shipSinFrequency       = 0.1f;
    float             shipCircleRadius       = 100.f;
    float             shipAngularSpeed       = 1.0f;
    bool              shipsTrail             = false;
    int               shipsTrailSize         = 100;
    std::list<ImVec2> trail(shipsTrailSize);

    std::vector<ImVec2> planes;

    bool   displayGrid     = true;
    bool   displayMarkings = true;
    int    gridStep        = 64;
    ImVec4 gridColor(0.5f, 0.5f, 0.5f, 1.0f);
    ImVec4 markingsColor(1.0f, 1.0f, 1.0f, 1.0f);

    bool   displayBorder = true;
    ImVec4 borderColor(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 backgroundColor(0.2f, 0.2f, 0.2f, 1.0f);

    bool enableDemoWindow  = false;
    bool enableDebugWindow = false;

    float zoomScale = 1.0f;

    std::atomic_bool stopShipAnimationThreadExecution(false);
    std::atomic_bool isShipAnimationThreadRunning(false);
    bool             animateShip = false;
    std::thread      shipAnimationThread;

    std::atomic_bool stopPlanesAnimationThreadExecution(false);
    std::atomic_bool isPlanesAnimationThreadRunning(false);
    bool animatePlanes = false;
    std::thread      planesAnimationThread;

    auto guiFunction = [&]() {
        if (!isShipAnimationThreadRunning) {
            shipAnimationThread = std::thread([&]() {
                isShipAnimationThreadRunning = true;

                while (!stopShipAnimationThreadExecution) {
                    if (animateShip) {
                        switch (animationSelectedIndex) {
                            case 0: // Linear
                                shipCoordinates.x += shipLinearSpeedX;
                                shipCoordinates.y += shipLinearSpeedY;
                                break;
                            case 1: // Sin
                                shipCoordinates.x += shipLinearSpeedX;
                                shipCoordinates.y =
                                    shipSinAmplitude * std::sin(2 * M_PI * shipSinFrequency * ImGui::GetTime());
                                break;
                            case 2: // Circle
                                double time = ImGui::GetTime();
                                shipCoordinates.x =
                                    shipCircleRadius * std::cos(shipAngularSpeed * time) + (shipLinearSpeedX * time);
                                shipCoordinates.y =
                                    shipCircleRadius * std::sin(shipAngularSpeed * time) + (shipLinearSpeedY * time);
                        }
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(shipAccelerationPeriod));
                }
            });
        }

        if (!isPlanesAnimationThreadRunning) {
            planesAnimationThread = std::thread([&]() {
                isPlanesAnimationThreadRunning = true;

                while (!stopPlanesAnimationThreadExecution) {
                    if (animatePlanes) {
                        float centerOfOrbitX = shipCoordinates.x;
                        float centerOfOrbitY = shipCoordinates.y;
                        float radius = 200.0f;
                        float ownRadius = 100.0f;
                        float angularSpeed = 2.0f;
                        float ownAngularSpeed = 8.0f;

                        for (auto& plane : planes) {
                            float angle = ImGui::GetTime() * angularSpeed;
                            float orbitX = centerOfOrbitX + radius * std::cos(angle);
                            float orbitY = centerOfOrbitY + radius * std::sin(angle);

                            float ownAngle = ImGui::GetTime() * ownAngularSpeed;
                            plane.x = orbitX + ownRadius * std::cos(ownAngle);
                            plane.y = orbitY + ownRadius * std::sin(ownAngle);
                        }
                    }
                }
            });
        }

        ImGui::Begin("Settings");
        if (ImGui::CollapsingHeader("Ship", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::ColorEdit3("Ship color", (float*) &shipColor);
            if (ImGui::Button("Reset ship")) {
                shipCoordinates.x = 0.0f;
                shipCoordinates.y = 0.0f;
            }
            ImGui::Checkbox("Animate ship", &animateShip);
            if (ImGui::BeginListBox("Animations")) {
                for (int i = 0; i < IM_ARRAYSIZE(animations); i++) {
                    const bool isSelected = animationSelectedIndex == i;

                    if (ImGui::Selectable(animations[i], isSelected)) {
                        animationSelectedIndex = i;
                        animateShip            = false;
                    }

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::SeparatorText("Animation");
            switch (animationSelectedIndex) {
                case 0: // Linear
                    ImGui::InputFloat("Ship linear speed X", &shipLinearSpeedX);
                    ImGui::InputFloat("Ship linear speed Y", &shipLinearSpeedY);
                    break;
                case 1: // Sin
                    ImGui::InputFloat("Ship linear speed X", &shipLinearSpeedX);
                    ImGui::InputFloat("Ship sin amplitude", &shipSinAmplitude);
                    ImGui::InputFloat("Ship sin frequency", &shipSinFrequency);
                    break;
                case 2: // Circle
                    ImGui::InputFloat("Ship linear speed X", &shipLinearSpeedX);
                    ImGui::InputFloat("Ship linear speed Y", &shipLinearSpeedY);
                    ImGui::InputFloat("Ship circle radius", &shipCircleRadius);
                    ImGui::InputFloat("Ship angular speed", &shipAngularSpeed);
                    break;
                default:
                    ImGui::Text("Unsupported yet");
                    break;
            }
            ImGui::Separator();
            if (ImGui::InputInt("Ship acceleration period", &shipAccelerationPeriod)) {
                if (shipAccelerationPeriod < 1) {
                    shipAccelerationPeriod = 1;
                }
            }
            ImGui::Checkbox("Follow ship", &followShip);
            if (ImGui::Checkbox("Ship's trail", &shipsTrail)) {
                trail.clear();
            }
            if (shipsTrail) {
                if (ImGui::InputInt("Ship's trail size", &shipsTrailSize)) {
                    if (shipsTrailSize > 0) {
                        trail.resize(shipsTrailSize);
                    }
                }
            }
        }
        if (ImGui::CollapsingHeader("Planes", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("Add")) {
                if (planes.empty()) {
                    planes.emplace_back(0.0f, 0.0f);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove")) {
                if (!planes.empty()) {
                    planes.pop_back();
                }
            }
            ImGui::Checkbox("Animate planes", &animatePlanes);
            ImGui::Text("Number of planes: %zu", planes.size());
        }
        if (ImGui::CollapsingHeader("Grid", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Display grid", &displayGrid);
            ImGui::Checkbox("Display markings", &displayMarkings);
            ImGui::InputInt("Grid step", &gridStep);
            ImGui::ColorEdit3("Grid color", (float*) &gridColor);
            ImGui::ColorEdit3("Markings color", (float*) &markingsColor);
        }
        if (ImGui::CollapsingHeader("Background", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Display border", &displayBorder);
            ImGui::ColorEdit3("Border color", (float*) &borderColor);
            ImGui::ColorEdit3("Background color", (float*) &backgroundColor);
        }
        if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_DefaultOpen)) {
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
        ImGuiIO&   io        = ImGui::GetIO();
        const bool isActive  = ImGui::IsItemActive();
        bool       isHovered = ImGui::IsItemHovered();

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

        const ImVec2 origin(canvas_p0.x + scrollingOffset.x, canvas_p0.y + scrollingOffset.y);

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
                    float fakeX = (x0 - origin.x) / zoomScale;
                    ImGui::TextColored(markingsColor, "%.0f", fakeX);
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
                    ImGui::TextColored(markingsColor, "%.0f", fakeY);
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
            ImGui::Text("Mouse Delta X: %.0f", io.MouseDelta.x);
            ImGui::SameLine();
            ImGui::Text("Mouse Delta Y: %.0f", io.MouseDelta.y);
            ImGui::Text("Actual Ship X: %.0f", shipCoordinates.x + origin.x);
            ImGui::SameLine();
            ImGui::Text("Actual Ship Y: %.0f", shipCoordinates.y + origin.y);
            ImGui::Text("Time: %.3f", ImGui::GetTime());
            ImGui::End();
        }

        if (enableDemoWindow) {
            ImGui::ShowDemoWindow();
        }

        ImVec2 shipToDraw = ImVec2(shipCoordinates.x + origin.x, shipCoordinates.y + origin.y);

        if (shipsTrail) {
            if (trail.size() >= shipsTrailSize) {
                trail.pop_back();
            }
            if (trail.front().x != shipToDraw.x || trail.front().y != shipToDraw.y) {
                trail.push_front(shipToDraw);
            }

            float fadeFactor = 0.0f;
            for (const auto& trailItem: trail) {
                shipColor.w = (trail.size() - fadeFactor++) / trail.size(); // shipColor.w -> alpha
                draw_list->AddCircleFilled(trailItem, 10 * zoomScale, ImColor(shipColor));
            }
        } else {
            draw_list->AddCircleFilled(shipToDraw, 10 * zoomScale, ImColor(shipColor));
        }

        for (const auto& plane : planes) {
            ImVec2 planeToDraw(plane.x + origin.x, plane.y + origin.y);
            draw_list->AddCircleFilled(planeToDraw, 5 * zoomScale, ImColor(shipColor));
        }

        draw_list->PopClipRect();
    };

    const std::string            windowTitle             = "Test Scrolling Pane";
    const HelloImGui::ScreenSize screenSize              = { 1280, 720 };
    const bool                   autoSizeWindow          = false;
    const bool                   restorePreviousGeometry = false;
    const float                  fpsIdle                 = 0.0f;

    HelloImGui::Run(guiFunction, windowTitle, autoSizeWindow, restorePreviousGeometry, screenSize, fpsIdle);

    stopShipAnimationThreadExecution = true;
    stopPlanesAnimationThreadExecution = true;

    shipAnimationThread.join();
    planesAnimationThread.join();

    return 0;
}
