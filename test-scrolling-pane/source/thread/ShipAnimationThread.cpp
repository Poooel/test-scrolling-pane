#include "thread/ShipAnimationThread.h"

void ShipAnimationThread::run(const std::stop_token& stoken, ApplicationState& applicationState) {
    while (!stoken.stop_requested()) {
        double time = ImGui::GetTime();

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
                        static_cast<float>(std::sin(2 * M_PI * applicationState.shipSinFrequency * time));
                    break;
                case 2: // Circle
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
