#include "window/CanvasWindow.h"

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

    if (applicationState.displayGrid) {
        drawGrid(applicationState);
    }

    drawShip(applicationState);

    if (applicationState.animatePlanes) {
        drawPlanes(applicationState);
    }

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

    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    if (applicationState.followShip) {
        applicationState.scrollingOffset.x = (canvasSize.x / 2) - applicationState.shipCoordinates.x;
        applicationState.scrollingOffset.y = (canvasSize.y / 2) - applicationState.shipCoordinates.y;
    }

    ImGui::EndChild();

    //    ImVec2 canvasStart = ImGui::GetCursorScreenPos();
    //    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    //
    //    if (canvasSize.x < 50.0f) {
    //        canvasSize.x = 50.0f;
    //    }
    //    if (canvasSize.y < 50.0f) {
    //        canvasSize.y = 50.0f;
    //    }
    //
    //    ImVec2 canvasEnd = ImVec2(canvasStart.x + canvasSize.x, canvasStart.y + canvasSize.y);
    //
    //    ImDrawList* drawList = ImGui::GetWindowDrawList();
    //    drawList->AddRectFilled(canvasStart, canvasEnd, ImColor(applicationState.backgroundColor));
    //
    //    if (applicationState.displayBorder) {
    //        drawList->AddRect(canvasStart, canvasEnd, ImColor(applicationState.borderColor));
    //    }
    //
    //    ImGui::InvisibleButton("canvas", canvasSize, ImGuiButtonFlags_MouseButtonLeft);
    //    ImGuiIO&   io        = ImGui::GetIO();
    //    const bool isActive  = ImGui::IsItemActive();
    //    bool       isHovered = ImGui::IsItemHovered();
    //
    //    if (isHovered) {
    //        if (io.MouseWheel != 0.0f) {
    //            applicationState.zoomScale += io.MouseWheel * 0.1f;
    //            applicationState.zoomScale  = applicationState.zoomScale < 0.1f ? 0.1f : applicationState.zoomScale;
    //            applicationState.zoomScale  = applicationState.zoomScale > 2.0f ? 2.0f : applicationState.zoomScale;
    //        }
    //    }
    //
    //    if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) && !applicationState.followShip) {
    //        applicationState.scrollingOffset.x += io.MouseDelta.x;
    //        applicationState.scrollingOffset.y += io.MouseDelta.y;
    //    }
    //
    //    if (applicationState.followShip) {
    //        applicationState.scrollingOffset.x = (canvasSize.x / 2) - applicationState.shipCoordinates.x;
    //        applicationState.scrollingOffset.y = (canvasSize.y / 2) - applicationState.shipCoordinates.y;
    //    }
    //
    //    drawList->PushClipRect(canvasStart, canvasEnd, true);
    //
    //    const ImVec2 mapOrigin(canvasStart.x + applicationState.scrollingOffset.x, canvasStart.y +
    //    applicationState.scrollingOffset.y);
    //
    //    if (applicationState.displayGrid) {
    //        float zoomedGridStep = applicationState.gridStep * applicationState.zoomScale;
    //
    //        // vertical lines
    //        for (float x = fmodf(applicationState.scrollingOffset.x, zoomedGridStep); x < canvasSize.x; x +=
    //        zoomedGridStep) {
    //            float x0 = canvasStart.x + x;
    //            float y0 = canvasStart.y;
    //            float y1 = canvasEnd.y;
    //
    //            drawList->AddLine(ImVec2(x0, y0), ImVec2(x0, y1), ImColor(applicationState.gridColor));
    //
    //            if (applicationState.displayMarkings) {
    //                ImGui::SetCursorPos(ImVec2(x0 - 10, y0 + 5));
    //                float fakeX = (x0 - mapOrigin.x) / applicationState.zoomScale;
    //                ImGui::TextColored(applicationState.markingsColor, "%.0f", fakeX);
    //            }
    //        }
    //
    //        // horizontal lines
    //        for (float y = fmodf(applicationState.scrollingOffset.y, zoomedGridStep); y < canvasSize.y; y +=
    //        zoomedGridStep) {
    //            float x0 = canvasStart.x;
    //            float y0 = canvasStart.y + y;
    //            float x1 = canvasEnd.x;
    //
    //            drawList->AddLine(ImVec2(x0, y0), ImVec2(x1, y0), ImColor(applicationState.gridColor));
    //
    //            if (applicationState.displayMarkings) {
    //                ImGui::SetCursorPos(ImVec2(x0 + 5, y0));
    //                float fakeY = (y0 - mapOrigin.y) / applicationState.zoomScale;
    //                ImGui::TextColored(applicationState.markingsColor, "%.0f", fakeY);
    //            }
    //        }
    //    }
    //
    //    if (applicationState.shipsTrail) {
    //        if (applicationState.trail.size() >= applicationState.shipsTrailSize) {
    //            applicationState.trail.pop_back();
    //        }
    //        if (applicationState.trail.empty() || applicationState.trail.front().x !=
    //        applicationState.shipCoordinates.x || applicationState.trail.front().y !=
    //        applicationState.shipCoordinates.y) {
    //            applicationState.trail.emplace_front(applicationState.shipCoordinates.x,
    //            applicationState.shipCoordinates.y);
    //        }
    //
    //        float fadeFactor = 0.0f;
    //        for (const auto& trailItem: applicationState.trail) {
    //            float alpha = (applicationState.trail.size() - fadeFactor++) / applicationState.trail.size();
    //            drawList->AddCircleFilled(
    //                ImVec2(trailItem.x + mapOrigin.x, trailItem.y + mapOrigin.y),
    //                10 * applicationState.zoomScale,
    //                ImColor(applicationState.shipColor.x, applicationState.shipColor.y, applicationState.shipColor.z,
    //                alpha)
    //            );
    //        }
    //    } else {
    //        drawList->AddCircleFilled(
    //            ImVec2(applicationState.shipCoordinates.x + mapOrigin.x, applicationState.shipCoordinates.y +
    //            mapOrigin.y), 10 * applicationState.zoomScale, ImColor(applicationState.shipColor)
    //        );
    //    }
    //
    //    if (applicationState.animatePlanes) {
    //        for (const auto& plane: applicationState.planes) {
    //            drawList->AddCircleFilled(
    //                ImVec2(plane.x + mapOrigin.x, plane.y + mapOrigin.y), 5 * applicationState.zoomScale,
    //                ImColor(applicationState.shipColor)
    //            );
    //        }
    //    }
    //
    //    drawList->PopClipRect();
}

void CanvasWindow::drawGrid(ApplicationState& applicationState) {
    ImVec2 canvasStart = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize  = ImGui::GetContentRegionAvail();
    ImVec2 canvasEnd(canvasStart.x + canvasSize.x, canvasStart.y + canvasSize.y);
    ImVec2 mapOrigin(
        canvasStart.x + applicationState.scrollingOffset.x, canvasStart.y + applicationState.scrollingOffset.y
    );
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    drawList->PushClipRect(canvasStart, canvasEnd, true);

    auto adjustedGridStep = static_cast<float>(applicationState.gridStep);

    for (float x  = fmodf(applicationState.scrollingOffset.x, adjustedGridStep); x < canvasSize.x;
         x       += adjustedGridStep) {
        float x0           = canvasStart.x + x;
        float y0           = canvasStart.y;
        float y1           = canvasEnd.y;
        float markingValue = x0 - mapOrigin.x;

        if (std::round(markingValue) == 0.0f) {
            drawList->AddLine(ImVec2(x0, y0), ImVec2(x0, y1), ImColor(applicationState.gridColor), 5.0f);
        } else if (fmodf(markingValue, 500) == 0.0f) {
            drawList->AddLine(ImVec2(x0, y0), ImVec2(x0, y1), ImColor(applicationState.gridColor), 2.5f);
        } else {
            drawList->AddLine(ImVec2(x0, y0), ImVec2(x0, y1), ImColor(applicationState.gridColor));
        }

        if (applicationState.displayMarkings) {
            std::string marking = std::format("{:.0f}", markingValue);
            drawList->AddText(ImVec2(x0 - 10, y0 + 5), ImColor(applicationState.markingsColor), marking.c_str());
        }
    }

    for (float y  = fmodf(applicationState.scrollingOffset.y, adjustedGridStep); y < canvasSize.y;
         y       += adjustedGridStep) {
        float x0           = canvasStart.x;
        float y0           = canvasStart.y + y;
        float x1           = canvasEnd.x;
        float markingValue = y0 - mapOrigin.y;

        if (std::round(markingValue) == 0.0f) {
            drawList->AddLine(ImVec2(x0, y0), ImVec2(x1, y0), ImColor(applicationState.gridColor), 5.0f);
        } else if (fmodf(markingValue, 500) == 0.0f) {
            drawList->AddLine(ImVec2(x0, y0), ImVec2(x1, y0), ImColor(applicationState.gridColor), 2.5f);
        } else {
            drawList->AddLine(ImVec2(x0, y0), ImVec2(x1, y0), ImColor(applicationState.gridColor));
        }

        if (applicationState.displayMarkings) {
            std::string marking = std::format("{:.0f}", markingValue);
            drawList->AddText(ImVec2(x0 + 5, y0), ImColor(applicationState.markingsColor), marking.c_str());
        }
    }

    drawList->PopClipRect();
}

void CanvasWindow::drawShip(ApplicationState& applicationState) {
    ImDrawList* drawList    = ImGui::GetWindowDrawList();
    ImVec2      canvasStart = ImGui::GetCursorScreenPos();
    ImVec2      mapOrigin(
        canvasStart.x + applicationState.scrollingOffset.x, canvasStart.y + applicationState.scrollingOffset.y
    );

    if (applicationState.shipsTrail) {
        if (applicationState.trail.size() >= applicationState.shipsTrailSize) {
            applicationState.trail.pop_back();
        }
        if (applicationState.trail.empty() || applicationState.trail.front().x != applicationState.shipCoordinates.x ||
            applicationState.trail.front().y != applicationState.shipCoordinates.y) {
            applicationState.trail.emplace_front(
                applicationState.shipCoordinates.x, applicationState.shipCoordinates.y
            );
        }

        float fadeFactor = 0.0f;
        for (const auto& trailItem: applicationState.trail) {
            float alpha = (applicationState.trail.size() - fadeFactor++) / applicationState.trail.size();
            drawList->AddCircleFilled(
                ImVec2(trailItem.x + mapOrigin.x, trailItem.y + mapOrigin.y),
                10,
                ImColor(applicationState.shipColor.x, applicationState.shipColor.y, applicationState.shipColor.z, alpha)
            );
        }
    } else {
        drawList->AddCircleFilled(
            ImVec2(applicationState.shipCoordinates.x + mapOrigin.x, applicationState.shipCoordinates.y + mapOrigin.y),
            10,
            ImColor(applicationState.shipColor)
        );
    }
}

void CanvasWindow::drawPlanes(ApplicationState& applicationState) {
    ImDrawList* drawList    = ImGui::GetWindowDrawList();
    ImVec2      canvasStart = ImGui::GetCursorScreenPos();
    ImVec2      mapOrigin(
        canvasStart.x + applicationState.scrollingOffset.x, canvasStart.y + applicationState.scrollingOffset.y
    );

    if (applicationState.animatePlanes) {
        for (const auto& plane: applicationState.planes) {
            drawList->AddCircleFilled(
                ImVec2(plane.x + mapOrigin.x, plane.y + mapOrigin.y), 5, ImColor(applicationState.shipColor)
            );
        }
    }
}
