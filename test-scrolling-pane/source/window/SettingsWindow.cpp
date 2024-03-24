#include "window/SettingsWindow.h"
#include "window/DebugWindow.h"

void SettingsWindow::gui(ApplicationState& applicationState) {
    ImGui::Begin("Settings");
    ImGui::PushFont(applicationState.font);

    if (ImGui::CollapsingHeader("Ship", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("Ship color", (float*) &applicationState.shipColor);
        if (ImGui::Button("Reset ship")) {
            applicationState.shipCoordinates.x = 0.0f;
            applicationState.shipCoordinates.y = 0.0f;
        }
        ImGui::Checkbox("Animate ship", &applicationState.animateShip);
        if (ImGui::BeginListBox("Ship animations")) {
            for (int i = 0; i < IM_ARRAYSIZE(applicationState.shipAnimations); i++) {
                const bool isSelected = applicationState.shipAnimationSelectedIndex == i;

                if (ImGui::Selectable(applicationState.shipAnimations[i], isSelected)) {
                    applicationState.shipAnimationSelectedIndex = i;
                    applicationState.animateShip                = false;
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }
        ImGui::SeparatorText("Animation");
        switch (applicationState.shipAnimationSelectedIndex) {
            case 0: // Linear
                ImGui::InputFloat("Ship linear speed X", &applicationState.shipLinearSpeedX);
                ImGui::InputFloat("Ship linear speed Y", &applicationState.shipLinearSpeedY);
                break;
            case 1: // Sin
                ImGui::InputFloat("Ship linear speed X", &applicationState.shipLinearSpeedX);
                ImGui::InputFloat("Ship sin amplitude", &applicationState.shipSinAmplitude);
                ImGui::InputFloat("Ship sin frequency", &applicationState.shipSinFrequency);
                break;
            case 2: // Circle
                ImGui::InputFloat("Ship linear speed X", &applicationState.shipLinearSpeedX);
                ImGui::InputFloat("Ship linear speed Y", &applicationState.shipLinearSpeedY);
                ImGui::InputFloat("Ship circle radius", &applicationState.shipCircleRadius);
                ImGui::InputFloat("Ship angular speed", &applicationState.shipAngularSpeed);
                break;
            default:
                ImGui::Text("Unsupported yet");
                break;
        }
        ImGui::Separator();
        if (ImGui::InputInt("Ship animation frequency", &applicationState.shipAnimationFrequency)) {
            if (applicationState.shipAnimationFrequency < 1) {
                applicationState.shipAnimationFrequency = 1;
            }
        }
        ImGui::Checkbox("Follow ship", &applicationState.followShip);
        if (ImGui::Checkbox("Ship's trail", &applicationState.shipsTrail)) {
            if (!applicationState.shipsTrail) {
                applicationState.trail.clear();
            }
        }
        if (applicationState.shipsTrail) {
            if (ImGui::InputInt("Ship's trail size", &applicationState.shipsTrailSize)) {
                if (applicationState.shipsTrailSize > 0) {
                    applicationState.trail.resize(applicationState.shipsTrailSize);
                }
            }
        }
    }
    if (ImGui::CollapsingHeader("Planes", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Add")) {
            if (applicationState.planesAnimationSelectedIndex == 4) { // Attack
                double theta = applicationState.randomZeroTwoPiDistribution(applicationState.randomGenerator);
                double x     = applicationState.planeSpawnRadius * std::cos(theta);
                double y     = applicationState.planeSpawnRadius * std::sin(theta);
                applicationState.planes.emplace_back(x, y);
            } else {
                applicationState.planes.emplace_back(0.0f, 0.0f);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove")) {
            if (!applicationState.planes.empty()) {
                applicationState.planes.pop_back();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove all")) {
            applicationState.planes.clear();
        }
        ImGui::Checkbox("Animate planes", &applicationState.animatePlanes);
        ImGui::Text("Number of planes: %zu", applicationState.planes.size());
        if (ImGui::BeginListBox("Planes animations")) {
            for (int i = 0; i < IM_ARRAYSIZE(applicationState.planesAnimations); i++) {
                const bool isSelected = applicationState.planesAnimationSelectedIndex == i;

                if (ImGui::Selectable(applicationState.planesAnimations[i], isSelected)) {
                    applicationState.planesAnimationSelectedIndex = i;
                    applicationState.animatePlanes                = false;
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }
        ImGui::SeparatorText("Animation");
        switch (applicationState.planesAnimationSelectedIndex) {
            case 0: // Orbit
                ImGui::InputFloat("Main orbit radius", &applicationState.planeMainOrbitRadius);
                ImGui::InputFloat("Main orbit angular speed", &applicationState.planeMainOrbitAngularSpeed);
                break;
            case 1: // Multiple orbits & secondaries
            case 2: // Single orbit & secondaries
                ImGui::InputFloat("Main orbit radius", &applicationState.planeMainOrbitRadius);
                ImGui::InputFloat("Main orbit angular speed", &applicationState.planeMainOrbitAngularSpeed);
                ImGui::InputFloat("Secondary orbit radius", &applicationState.planeSecondaryOrbitRadius);
                ImGui::InputFloat("Secondary orbit angular speed", &applicationState.planeSecondaryOrbitAngularSpeed);
                break;
            case 3: // Random
                ImGui::InputFloat("Plane speed", &applicationState.planeSpeed);
                break;
            case 4: // Attack
                ImGui::InputFloat("Spawn radius", &applicationState.planeSpawnRadius);
                ImGui::InputFloat("Attack speed", &applicationState.planeAttackSpeed);
                break;
            default:
                ImGui::Text("Unsupported yet");
                break;
        }
    }
    if (ImGui::CollapsingHeader("Grid", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Display grid", &applicationState.displayGrid);
        ImGui::Checkbox("Display markings", &applicationState.displayMarkings);
        ImGui::InputInt("Grid step", &applicationState.gridStep);
        ImGui::ColorEdit3("Grid color", (float*) &applicationState.gridColor);
        ImGui::ColorEdit3("Markings color", (float*) &applicationState.markingsColor);
    }
    if (ImGui::CollapsingHeader("Background", ImGuiTreeNodeFlags_DefaultOpen)) {
        // ImGui::Checkbox("Display border", &applicationState.displayBorder);
        // ImGui::ColorEdit3("Border color", (float*) &applicationState.borderColor);
        ImGui::ColorEdit3("Background color", (float*) &applicationState.backgroundColor);
    }
    if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Enable Demo Window", &applicationState.enableDemoWindow);
        ImGui::Checkbox("Enable Debug Window", &applicationState.enableDebugWindow);
        if (applicationState.enableDemoWindow) {
            ImGui::ShowDemoWindow();
        }
        if (applicationState.enableDebugWindow) {
            DebugWindow::gui(applicationState);
        }
    }

    ImGui::PopFont();
    ImGui::End();
}
