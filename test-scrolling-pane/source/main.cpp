#include "state/ApplicationState.h"
#include "thread/PlanesAnimationThread.h"
#include "thread/ShipAnimationThread.h"
#include "window/Docking.h"

int main(int, char*[]) {
    // TODO: add speed vector north + east
    // TODO: convert from x/y to lat/long
    // TODO: trail for planes
    // TODO: fix zoomScale, grid step should always be 64 (when zoomed in, 1 pixel equals "more" X)
    //       zoomScale = 1.0 --- 1 pixel = 1 X
    //       zoomScale = 0.1 --- 1 pixel = 10 X
    //       zoomScale = 2.0 --- 1 pixel = 0.5 X

    ApplicationState applicationState;

    HelloImGui::RunnerParams runnerParams;
    runnerParams.appWindowParams.windowTitle                   = applicationState.windowTitle;
    runnerParams.appWindowParams.windowGeometry.fullScreenMode = HelloImGui::FullScreenMode::FullMonitorWorkArea;
    runnerParams.appWindowParams.restorePreviousGeometry       = true;

    runnerParams.imGuiWindowParams.showStatusBar = true;

    auto& tweakedTheme           = runnerParams.imGuiWindowParams.tweakedTheme;
    tweakedTheme.Theme           = ImGuiTheme::ImGuiTheme_Darcula;
    tweakedTheme.Tweaks.Rounding = 2.0f;

    runnerParams.imGuiWindowParams.defaultImGuiWindowType =
        HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;
    runnerParams.imGuiWindowParams.enableViewports = true;
    runnerParams.dockingParams                     = Docking::createDefaultLayout(applicationState);

    runnerParams.iniFolderType = HelloImGui::IniFolderType::AppUserConfigFolder;
    runnerParams.iniFilename   = "test_scrolling_pane/settings.ini";

    runnerParams.callbacks.EnqueuePostInit([&]() {
        applicationState.shipAnimationThread   = std::jthread(ShipAnimationThread::run, std::ref(applicationState));
        applicationState.planesAnimationThread = std::jthread(PlanesAnimationThread::run, std::ref(applicationState));
    });

    runnerParams.callbacks.EnqueueBeforeExit([&]() {
        applicationState.shipAnimationThread.request_stop();
        applicationState.shipAnimationThread.join();
        applicationState.planesAnimationThread.request_stop();
        applicationState.planesAnimationThread.join();
    });

    HelloImGui::DeleteIniSettings(runnerParams);

    HelloImGui::Run(runnerParams);

    return 0;
}
