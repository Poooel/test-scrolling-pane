#include "hello_imgui/hello_imgui.h"
#include "perlin.h"

#include <cmath>
#include <list>
#include <random>
#include <thread>

int main(int, char*[]) {
    // TODO: add speed vector north + east
    // TODO: convert from x/y to lat/long
    // TODO: rework drawing + scrolling + origin (?)
    // TODO: have own timers to avoid jumps
    // TODO: break up the code
    // TODO: trail for planes
    // TODO: rework window layout

    const std::string            windowTitle             = "Test Scrolling Pane";
    const HelloImGui::ScreenSize screenSize              = { 1280, 720 };
    const bool                   autoSizeWindow          = false;
    const bool                   restorePreviousGeometry = false;
    const float                  fpsIdle                 = 0.0f;

    std::random_device               randomDevice;
    std::mt19937                     generator(randomDevice());
    std::uniform_real_distribution<> distribution(0, 2 * M_PI);

    ImVec2 shipCoordinates(0.0f, 0.0f);
    ImVec2 scrollingOffset(0.0f, 0.0f);

    ImVec4            shipColor(1.0f, 1.0f, 1.0f, 1.0f);
    float             shipLinearSpeedX           = 0.0f;
    float             shipLinearSpeedY           = 0.0f;
    int               shipAccelerationPeriod     = 10;
    bool              followShip                 = true;
    const char*       shipAnimations[]           = { "Linear", "Sin", "Circle" };
    int               shipAnimationSelectedIndex = 0;
    float             shipSinAmplitude           = 100.0f;
    float             shipSinFrequency           = 0.1f;
    float             shipCircleRadius           = 100.f;
    float             shipAngularSpeed           = 1.0f;
    bool              shipsTrail                 = false;
    int               shipsTrailSize             = 100;
    std::list<ImVec2> trail;

    std::list<ImVec2> planes;
    float             mainOrbitRadius            = 200.0f;
    float             secondaryOrbitRadius       = 100.0f;
    float             mainOrbitAngularSpeed      = 2.0f;
    float             secondaryOrbitAngularSpeed = 8.0f;
    const char*       planesAnimations[]         = {
        "Orbit", "Multiple orbits & secondaries", "Single orbit & secondaries", "Random", "Attack"
    };
    int   planesAnimationSelectedIndex = 0;
    float attackSpeed                  = 3.0f;
    float spawnRadius                  = 150.0f;
    float planeSpeed                   = 1.0f;

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
    bool             animatePlanes = false;
    std::thread      planesAnimationThread;

    auto guiFunction = [&]() {
        if (!isShipAnimationThreadRunning) {
            shipAnimationThread = std::thread([&]() {
                isShipAnimationThreadRunning = true;

                while (!stopShipAnimationThreadExecution) {
                    if (animateShip) {
                        switch (shipAnimationSelectedIndex) {
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
                        int planeIndex = 0;

                        for (auto plane = planes.begin(); plane != planes.end();) {
                            switch (planesAnimationSelectedIndex) {
                                case 0: // Orbit
                                    {
                                        float angle = ImGui::GetTime() * mainOrbitAngularSpeed +
                                                      (planeIndex * (2.0f * M_PI / planes.size()));
                                        plane->x = shipCoordinates.x + mainOrbitRadius * std::cos(angle);
                                        plane->y = shipCoordinates.y + mainOrbitRadius * std::sin(angle);
                                    }
                                    break;
                                case 1: // Multiple orbits & secondaries
                                    {
                                        float mainAngle = ImGui::GetTime() * mainOrbitAngularSpeed +
                                                          (planeIndex * (2.0f * M_PI / planes.size()));
                                        float mainOrbitCenterX =
                                            shipCoordinates.x + mainOrbitRadius * std::cos(mainAngle);
                                        float mainOrbitCenterY =
                                            shipCoordinates.y + mainOrbitRadius * std::sin(mainAngle);

                                        float secondaryAngle = ImGui::GetTime() * secondaryOrbitAngularSpeed;
                                        plane->x = mainOrbitCenterX + secondaryOrbitRadius * std::cos(secondaryAngle);
                                        plane->y = mainOrbitCenterY + secondaryOrbitRadius * std::sin(secondaryAngle);
                                    }
                                    break;
                                case 2: // Single orbit & secondaries
                                    {
                                        float mainAngle = ImGui::GetTime() * mainOrbitAngularSpeed;
                                        float mainOrbitCenterX =
                                            shipCoordinates.x + mainOrbitRadius * std::cos(mainAngle);
                                        float mainOrbitCenterY =
                                            shipCoordinates.y + mainOrbitRadius * std::sin(mainAngle);

                                        float secondaryAngle = ImGui::GetTime() * secondaryOrbitAngularSpeed +
                                                               (planeIndex * (2.0f * M_PI / planes.size()));
                                        plane->x = mainOrbitCenterX + secondaryOrbitRadius * std::cos(secondaryAngle);
                                        plane->y = mainOrbitCenterY + secondaryOrbitRadius * std::sin(secondaryAngle);
                                    }
                                    break;
                                case 3: // Random
                                    {
                                        double noiseValue =
                                            pnoise(plane->x / screenSize[0], plane->y / screenSize[1], planeIndex);
                                        plane->x += std::cos(noiseValue * 2.0 * M_PI) * planeSpeed;
                                        plane->y += std::sin(noiseValue * 2.0 * M_PI) * planeSpeed;
                                    }
                                    break;
                                case 4: // Attack
                                    {
                                        float directionX = shipCoordinates.x - plane->x;
                                        float directionY = shipCoordinates.y - plane->y;

                                        double normalizedVector =
                                            std::sqrt(std::pow(directionX, 2) + std::pow(directionY, 2));

                                        float normalizedDirectionX = directionX * (1 / normalizedVector);
                                        float normalizedDirectionY = directionY * (1 / normalizedVector);

                                        plane->x += normalizedDirectionX * attackSpeed;
                                        plane->y += normalizedDirectionY * attackSpeed;
                                    }
                                    break;
                            }

                            if (std::round(plane->x) == std::round(shipCoordinates.x) &&
                                std::round(plane->y) == std::round(shipCoordinates.y)) {
                                plane = planes.erase(plane);
                            } else {
                                plane++;
                            }

                            planeIndex++;
                        }
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(shipAccelerationPeriod));
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
            if (ImGui::BeginListBox("Ship animations")) {
                for (int i = 0; i < IM_ARRAYSIZE(shipAnimations); i++) {
                    const bool isSelected = shipAnimationSelectedIndex == i;

                    if (ImGui::Selectable(shipAnimations[i], isSelected)) {
                        shipAnimationSelectedIndex = i;
                        animateShip                = false;
                    }

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::SeparatorText("Animation");
            switch (shipAnimationSelectedIndex) {
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
                if (!shipsTrail) {
                    trail.clear();
                }
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
                if (planesAnimationSelectedIndex == 4) { // Attack
                    double theta = distribution(generator);
                    double x     = spawnRadius * std::cos(theta);
                    double y     = spawnRadius * std::sin(theta);
                    planes.emplace_back(x, y);
                } else {
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
            if (ImGui::BeginListBox("Planes animations")) {
                for (int i = 0; i < IM_ARRAYSIZE(planesAnimations); i++) {
                    const bool isSelected = planesAnimationSelectedIndex == i;

                    if (ImGui::Selectable(planesAnimations[i], isSelected)) {
                        planesAnimationSelectedIndex = i;
                        animatePlanes                = false;
                    }

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();
            }
            ImGui::SeparatorText("Animation");
            switch (planesAnimationSelectedIndex) {
                case 0: // Orbit
                    ImGui::InputFloat("Main orbit radius", &mainOrbitRadius);
                    ImGui::InputFloat("Main orbit angular speed", &mainOrbitAngularSpeed);
                    break;
                case 1: // Multiple orbits & secondaries
                case 2: // Single orbit & secondaries
                    ImGui::InputFloat("Main orbit radius", &mainOrbitRadius);
                    ImGui::InputFloat("Main orbit angular speed", &mainOrbitAngularSpeed);
                    ImGui::InputFloat("Secondary orbit radius", &secondaryOrbitRadius);
                    ImGui::InputFloat("Secondary orbit angular speed", &secondaryOrbitAngularSpeed);
                    break;
                case 3: // Random
                    ImGui::InputFloat("Plane speed", &planeSpeed);
                    break;
                case 4: // Attack
                    ImGui::InputFloat("Spawn radius", &spawnRadius);
                    ImGui::InputFloat("Attack speed", &attackSpeed);
                    break;
                default:
                    ImGui::Text("Unsupported yet");
                    break;
            }
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
            ImGui::Text("Trail size: %zu", trail.size());
            ImGui::End();
        }

        if (enableDemoWindow) {
            ImGui::ShowDemoWindow();
        }

        if (shipsTrail) {
            if (trail.size() >= shipsTrailSize) {
                trail.pop_back();
            }
            if (trail.empty() || trail.front().x != shipCoordinates.x || trail.front().y != shipCoordinates.y) {
                trail.emplace_front(shipCoordinates.x, shipCoordinates.y);
            }

            float fadeFactor = 0.0f;
            for (const auto& trailItem: trail) {
                float alpha = (trail.size() - fadeFactor++) / trail.size();
                draw_list->AddCircleFilled(
                    ImVec2(trailItem.x + origin.x, trailItem.y + origin.y),
                    10 * zoomScale,
                    ImColor(shipColor.x, shipColor.y, shipColor.z, alpha)
                );
            }
        } else {
            draw_list->AddCircleFilled(
                ImVec2(shipCoordinates.x + origin.x, shipCoordinates.y + origin.y), 10 * zoomScale, ImColor(shipColor)
            );
        }

        if (animatePlanes) {
            for (const auto& plane: planes) {
                draw_list->AddCircleFilled(
                    ImVec2(plane.x + origin.x, plane.y + origin.y), 5 * zoomScale, ImColor(shipColor)
                );
            }
        }

        draw_list->PopClipRect();
    };

    HelloImGui::Run(guiFunction, windowTitle, autoSizeWindow, restorePreviousGeometry, screenSize, fpsIdle);

    stopShipAnimationThreadExecution   = true;
    stopPlanesAnimationThreadExecution = true;

    shipAnimationThread.join();
    planesAnimationThread.join();

    return 0;
}
