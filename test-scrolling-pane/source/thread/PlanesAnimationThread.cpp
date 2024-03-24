#include "thread/PlanesAnimationThread.h"

#include "misc/perlin.h"

void PlanesAnimationThread::run(ApplicationState& applicationState) {
    applicationState.isPlanesAnimationThreadRunning = true;

    while (!applicationState.stopPlanesAnimationThreadExecution) {
        if (applicationState.animatePlanes) {
            int planeIndex = 0;

            for (auto plane = applicationState.planes.begin(); plane != applicationState.planes.end();) {
                switch (applicationState.planesAnimationSelectedIndex) {
                    case 0: // Orbit
                        {
                            auto angle = static_cast<float>(
                                ImGui::GetTime() * applicationState.planeMainOrbitAngularSpeed +
                                (planeIndex * (2.0f * M_PI / static_cast<double>(applicationState.planes.size())))
                            );

                            plane->x = applicationState.shipCoordinates.x +
                                       applicationState.planeMainOrbitRadius * std::cos(angle);
                            plane->y = applicationState.shipCoordinates.y +
                                       applicationState.planeMainOrbitRadius * std::sin(angle);
                        }
                        break;
                    case 1: // Multiple orbits & secondaries
                        {
                            auto mainAngle = static_cast<float>(
                                ImGui::GetTime() * applicationState.planeMainOrbitAngularSpeed +
                                (planeIndex * (2.0f * M_PI / static_cast<double>(applicationState.planes.size())))
                            );

                            float mainOrbitCenterX = applicationState.shipCoordinates.x +
                                                     applicationState.planeMainOrbitRadius * std::cos(mainAngle);
                            float mainOrbitCenterY = applicationState.shipCoordinates.y +
                                                     applicationState.planeMainOrbitRadius * std::sin(mainAngle);

                            auto secondaryAngle =
                                static_cast<float>(ImGui::GetTime() * applicationState.planeSecondaryOrbitAngularSpeed);

                            plane->x = mainOrbitCenterX +
                                       applicationState.planeSecondaryOrbitRadius * std::cos(secondaryAngle);
                            plane->y = mainOrbitCenterY +
                                       applicationState.planeSecondaryOrbitRadius * std::sin(secondaryAngle);
                        }
                        break;
                    case 2: // Single orbit & secondaries
                        {
                            auto mainAngle =
                                static_cast<float>(ImGui::GetTime() * applicationState.planeMainOrbitAngularSpeed);
                            float mainOrbitCenterX = applicationState.shipCoordinates.x +
                                                     applicationState.planeMainOrbitRadius * std::cos(mainAngle);
                            float mainOrbitCenterY = applicationState.shipCoordinates.y +
                                                     applicationState.planeMainOrbitRadius * std::sin(mainAngle);

                            auto secondaryAngle = static_cast<float>(
                                ImGui::GetTime() * applicationState.planeSecondaryOrbitAngularSpeed +
                                (planeIndex * (2.0f * M_PI / static_cast<double>(applicationState.planes.size())))
                            );
                            plane->x = mainOrbitCenterX +
                                       applicationState.planeSecondaryOrbitRadius * std::cos(secondaryAngle);
                            plane->y = mainOrbitCenterY +
                                       applicationState.planeSecondaryOrbitRadius * std::sin(secondaryAngle);
                        }
                        break;
                    case 3: // Random
                        {
                            double noiseValue = pnoise(
                                plane->x / static_cast<float>(applicationState.screenSize[0]),
                                plane->y / static_cast<float>(applicationState.screenSize[1]),
                                planeIndex
                            );
                            plane->x +=
                                static_cast<float>(std::cos(noiseValue * 2.0 * M_PI) * applicationState.planeSpeed);
                            plane->y +=
                                static_cast<float>(std::sin(noiseValue * 2.0 * M_PI) * applicationState.planeSpeed);
                        }
                        break;
                    case 4: // Attack
                        {
                            float directionX = applicationState.shipCoordinates.x - plane->x;
                            float directionY = applicationState.shipCoordinates.y - plane->y;

                            double normalizedVector = std::sqrt(std::pow(directionX, 2) + std::pow(directionY, 2));

                            double normalizedDirectionX = directionX * (1 / normalizedVector);
                            double normalizedDirectionY = directionY * (1 / normalizedVector);

                            plane->x += static_cast<float>(normalizedDirectionX * applicationState.planeAttackSpeed);
                            plane->y += static_cast<float>(normalizedDirectionY * applicationState.planeAttackSpeed);
                        }
                        break;
                }

                if (std::round(plane->x) == std::round(applicationState.shipCoordinates.x) &&
                    std::round(plane->y) == std::round(applicationState.shipCoordinates.y)) {
                    plane = applicationState.planes.erase(plane);
                } else {
                    plane++;
                }

                planeIndex++;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(applicationState.planesAnimationFrequency));
    }
}
