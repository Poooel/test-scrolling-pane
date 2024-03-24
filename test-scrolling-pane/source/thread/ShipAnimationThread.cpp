#include "thread/ShipAnimationThread.h"

void ShipAnimationThread::run(ApplicationState& applicationState) {
    applicationState.isShipAnimationThreadRunning = true;

    while (!applicationState.stopShipAnimationThreadExecution) {
        if (applicationState.animateShip) {
            switch (applicationState.shipAnimationSelectedIndex) {
                case 0: // Linear
                    applicationState.shipCoordinates.x += applicationState.shipLinearSpeedX;
                    applicationState.shipCoordinates.y += applicationState.shipLinearSpeedY;
                    break;
                case 1: // Sin
                    applicationState.shipCoordinates.x += applicationState.shipLinearSpeedX;
                    applicationState.shipCoordinates.y =
                        applicationState.shipSinAmplitude *
                        static_cast<float>(std::sin(2 * M_PI * applicationState.shipSinFrequency * ImGui::GetTime()));
                    break;
                case 2: // Circle
                    double time = ImGui::GetTime();

                    applicationState.shipCoordinates.x = static_cast<float>(
                        applicationState.shipCircleRadius * std::cos(applicationState.shipAngularSpeed * time) +
                        (applicationState.shipLinearSpeedX * time)
                    );
                    applicationState.shipCoordinates.y = static_cast<float>(
                        applicationState.shipCircleRadius * std::sin(applicationState.shipAngularSpeed * time) +
                        (applicationState.shipLinearSpeedY * time)
                    );
                    break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(applicationState.shipAnimationFrequency));
    }
}
