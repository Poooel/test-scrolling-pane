#ifndef TEST_SCROLLING_PANE_DOCKING_H
#define TEST_SCROLLING_PANE_DOCKING_H

#include "state/ApplicationState.h"
#include "window/CanvasWindow.h"
#include "window/SettingsWindow.h"

class Docking {
    public:
        static HelloImGui::DockingParams createDefaultLayout(ApplicationState& applicationState);
        static std::vector<HelloImGui::DockingSplit> createDefaultSplit();
        static std::vector<HelloImGui::DockableWindow> createDockableWindows(ApplicationState& applicationState);
};

#endif // TEST_SCROLLING_PANE_DOCKING_H
