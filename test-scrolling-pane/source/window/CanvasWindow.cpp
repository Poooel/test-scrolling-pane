#include "window/CanvasWindow.h"

#include "state/CanvasState.h"

#include <format>

void CanvasWindow::gui(ApplicationState& applicationState) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, applicationState.backgroundColor);
    ImGui::BeginChild(
        "CanvasChild",
        ImVec2(0.0f, 0.0f),
        ImGuiChildFlags_Border,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    );
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGuiIO& io        = ImGui::GetIO();
    bool     isActive  = ImGui::IsWindowFocused();
    bool     isHovered = ImGui::IsWindowHovered();

    if (isActive && isHovered && ImGui::IsMouseDragging(0, 0.0f)) {
        applicationState.scrollingOffset.x += io.MouseDelta.x;
        applicationState.scrollingOffset.y += io.MouseDelta.y;
    }

    if (isHovered) {
        if (io.MouseWheel != 0.0f) {
            applicationState.zoomScale += io.MouseWheel * 0.1f;
            applicationState.zoomScale  = applicationState.zoomScale < 0.1f ? 0.1f : applicationState.zoomScale;
            applicationState.zoomScale  = applicationState.zoomScale > 2.0f ? 2.0f : applicationState.zoomScale;
        }
    }

    if (applicationState.followShip) {
        applicationState.scrollingOffset.x =
            (ImGui::GetContentRegionAvail().x / 2) - applicationState.shipCoordinates.x;
        applicationState.scrollingOffset.y =
            (ImGui::GetContentRegionAvail().y / 2) - applicationState.shipCoordinates.y;
    }

    CanvasState canvasState { applicationState };

    if (applicationState.displayGrid) {
        drawGrid(canvasState);
    }

    drawShip(canvasState);

    if (applicationState.animatePlanes) {
        drawPlanes(canvasState);
    }

    ImGui::EndChild();
}

void CanvasWindow::drawGrid(CanvasState& canvasState) {
    canvasState.drawList->PushClipRect(canvasState.canvasStart, canvasState.canvasEnd, true);

    auto adjustedGridStep = static_cast<float>(canvasState.applicationState.gridStep);

    for (float x = fmodf(canvasState.applicationState.scrollingOffset.x, adjustedGridStep);
         x < canvasState.canvasSize.x;
         x += adjustedGridStep) {
        float x0           = canvasState.canvasStart.x + x;
        float y0           = canvasState.canvasStart.y;
        float y1           = canvasState.canvasEnd.y;
        float markingValue = x0 - canvasState.mapOrigin.x;
        auto  markingColor = ImColor(canvasState.applicationState.gridColor);
        auto  p1           = ImVec2(x0, y0);
        auto  p2           = ImVec2(x0, y1);

        if (std::round(markingValue) == 0.0f) {
            canvasState.drawList->AddLine(p1, p2, markingColor, 5.0f);
        } else if (fmodf(markingValue, 500) == 0.0f) {
            canvasState.drawList->AddLine(p1, p2, markingColor, 2.5f);
        } else {
            canvasState.drawList->AddLine(p1, p2, markingColor);
        }

        if (canvasState.applicationState.displayMarkings) {
            std::string marking = std::format("{:.0f}", markingValue);
            canvasState.drawList->AddText(
                ImVec2(x0 - 10, y0 + 5), ImColor(canvasState.applicationState.markingsColor), marking.c_str()
            );
        }
    }

    for (float y = fmodf(canvasState.applicationState.scrollingOffset.y, adjustedGridStep);
         y < canvasState.canvasSize.y;
         y += adjustedGridStep) {
        float x0           = canvasState.canvasStart.x;
        float y0           = canvasState.canvasStart.y + y;
        float x1           = canvasState.canvasEnd.x;
        float markingValue = y0 - canvasState.mapOrigin.y;
        auto  markingColor = ImColor(canvasState.applicationState.gridColor);
        auto  p1           = ImVec2(x0, y0);
        auto  p2           = ImVec2(x1, y0);

        if (std::round(markingValue) == 0.0f) {
            canvasState.drawList->AddLine(p1, p2, markingColor, 5.0f);
        } else if (fmodf(markingValue, 500) == 0.0f) {
            canvasState.drawList->AddLine(p1, p2, markingColor, 2.5f);
        } else {
            canvasState.drawList->AddLine(p1, p2, markingColor);
        }

        if (canvasState.applicationState.displayMarkings) {
            std::string marking = std::format("{:.0f}", markingValue);
            canvasState.drawList->AddText(
                ImVec2(x0 + 5, y0), ImColor(canvasState.applicationState.markingsColor), marking.c_str()
            );
        }
    }

    canvasState.drawList->PopClipRect();
}

void CanvasWindow::drawShip(CanvasState& canvasState) {
    if (canvasState.applicationState.shipsTrail) {
        if (canvasState.applicationState.trail.size() >= canvasState.applicationState.shipsTrailSize) {
            canvasState.applicationState.trail.pop_back();
        }
        if (canvasState.applicationState.trail.empty() ||
            canvasState.applicationState.trail.front().x != canvasState.applicationState.shipCoordinates.x ||
            canvasState.applicationState.trail.front().y != canvasState.applicationState.shipCoordinates.y) {
            canvasState.applicationState.trail.emplace_front(
                canvasState.applicationState.shipCoordinates.x, canvasState.applicationState.shipCoordinates.y
            );
        }

        float fadeFactor = 0.0f;
        for (const auto& trailItem: canvasState.applicationState.trail) {
            float alpha =
                (canvasState.applicationState.trail.size() - fadeFactor++) / canvasState.applicationState.trail.size();
            canvasState.drawList->AddCircleFilled(
                ImVec2(trailItem.x + canvasState.mapOrigin.x, trailItem.y + canvasState.mapOrigin.y),
                10,
                ImColor(
                    canvasState.applicationState.shipColor.x,
                    canvasState.applicationState.shipColor.y,
                    canvasState.applicationState.shipColor.z,
                    alpha
                )
            );
        }
    } else {
        canvasState.drawList->AddCircleFilled(
            ImVec2(
                canvasState.applicationState.shipCoordinates.x + canvasState.mapOrigin.x,
                canvasState.applicationState.shipCoordinates.y + canvasState.mapOrigin.y
            ),
            10,
            ImColor(canvasState.applicationState.shipColor)
        );
    }
}

void CanvasWindow::drawPlanes(CanvasState& canvasState) {
    if (canvasState.applicationState.animatePlanes) {
        for (const auto& plane: canvasState.applicationState.planes) {
            canvasState.drawList->AddCircleFilled(
                ImVec2(plane.x + canvasState.mapOrigin.x, plane.y + canvasState.mapOrigin.y),
                5,
                ImColor(canvasState.applicationState.shipColor)
            );
        }
    }
}
