#ifndef TEST_SCROLLING_PANE_APPLICATIONSTATE_H
#define TEST_SCROLLING_PANE_APPLICATIONSTATE_H

#include "hello_imgui/hello_imgui.h"

#include <list>
#include <random>
#include <thread>

struct ApplicationState {
        // Window
        const std::string            windowTitle { "Test Scrolling Pane" };
        const HelloImGui::ScreenSize screenSize { 1280, 720 };

        // Random
        std::random_device               randomDevice;
        std::mt19937                     randomGenerator { randomDevice() };
        std::uniform_real_distribution<> randomZeroTwoPiDistribution { 0, 2 * M_PI };

        // Ship
        ImVec2 shipCoordinates { 0.0f, 0.0f };
        ImVec4 shipColor { 1.0f, 1.0f, 1.0f, 1.0f };
        float goToXCoordinate = 0.0f;
        float goToYCoordinate = 0.0f;

        // Ship's animations
        bool        animateShip { false };
        bool        followShip { true };
        const char* shipAnimations[3] { "Linear", "Sin", "Circle" };
        int         shipAnimationSelectedIndex { 0 };
        int         shipAnimationFrequency { 10 };

        // Ship's animation settings
        // Ship's animation: Linear
        float shipLinearSpeedX { 0.0f };
        float shipLinearSpeedY { 0.0f };

        // Ship's animation: Sin
        float shipSinAmplitude { 100.0f };
        float shipSinFrequency { 0.1f };

        // Ship's animation: Circle
        float shipCircleRadius { 100.f };
        float shipAngularSpeed { 1.0f };

        // Ship's trail
        bool              shipsTrail { false };
        int               shipsTrailSize { 100 };
        std::list<ImVec2> trail;

        // Planes
        std::list<ImVec2> planes;

        // Planes' animations
        bool        animatePlanes { false };
        const char* planesAnimations[5] {
            "Orbit", "Multiple orbits & secondaries", "Single orbit & secondaries", "Random", "Attack"
        };
        int planesAnimationSelectedIndex { 0 };
        int planesAnimationFrequency { 10 };

        // Planes' animation settings
        // Planes' animation: Orbit
        float planeMainOrbitRadius { 200.0f };
        float planeMainOrbitAngularSpeed { 2.0f };

        // Planes' animation: Multiple orbits & secondaries
        // Planes' animation: Single orbit & secondaries
        float planeSecondaryOrbitRadius { 100.0f };
        float planeSecondaryOrbitAngularSpeed { 8.0f };

        // Planes' animation: Random
        float planeSpeed { 1.0f };

        // Planes' animation: Attack
        float planeAttackSpeed { 3.0f };
        float planeSpawnRadius { 150.0f };

        // Background & Grid
        int    gridStep { 100 };
        bool   displayGrid { true };
        bool   displayMarkings { true };
        ImVec4 gridColor { 0.5f, 0.5f, 0.5f, 1.0f };
        ImVec4 markingsColor { 1.0f, 1.0f, 1.0f, 1.0f };
        ImVec4 backgroundColor { 0.2f, 0.2f, 0.2f, 1.0f };

        // Animation threads
        std::thread      shipAnimationThread;
        std::thread      planesAnimationThread;
        std::atomic_flag shipThreadRequestStop { false };
        std::atomic_flag planesThreadRequestStop { false };

        // Misc
        ImVec2 scrollingOffset { 0.0f, 0.0f };
        float  zoomScale { 1.0f };
        bool   enableDemoWindow { false };
        bool   enableDebugWindow { false };
};

#endif // TEST_SCROLLING_PANE_APPLICATIONSTATE_H
