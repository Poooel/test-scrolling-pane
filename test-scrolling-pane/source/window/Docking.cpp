#include "window/Docking.h"
#include "window/CanvasWindow.h"
#include "window/SettingsWindow.h"
#include "imgui_internal.h"

HelloImGui::DockingParams Docking::createDefaultLayout(ApplicationState& applicationState) {
    HelloImGui::DockingParams dockingParams;
    dockingParams.layoutName      = "Default";
    dockingParams.dockingSplits   = createDefaultSplit();
    dockingParams.dockableWindows = createDockableWindows(applicationState);
    dockingParams.mainDockSpaceNodeFlags = ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoTabBar;
    return dockingParams;
}

std::vector<HelloImGui::DockingSplit> Docking::createDefaultSplit() {
    HelloImGui::DockingSplit splitMainSettings;
    splitMainSettings.initialDock = "MainDockSpace";
    splitMainSettings.newDock     = "SettingsSpace";
    splitMainSettings.direction   = ImGuiDir_Left;
    splitMainSettings.ratio       = 0.20f;

    return { splitMainSettings };
}

std::vector<HelloImGui::DockableWindow> Docking::createDockableWindows(ApplicationState& applicationState) {
    HelloImGui::DockableWindow canvasWindow;
    canvasWindow.label = "Map";
    canvasWindow.dockSpaceName = "MainDockSpace";
    canvasWindow.imGuiWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    canvasWindow.GuiFunction = [&] { CanvasWindow::gui(applicationState); };

    HelloImGui::DockableWindow settingsWindow;
    settingsWindow.label = "Settings";
    settingsWindow.dockSpaceName = "SettingsSpace";
    settingsWindow.imGuiWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    settingsWindow.GuiFunction = [&] { SettingsWindow::gui(applicationState); };

    return { canvasWindow, settingsWindow };
}
