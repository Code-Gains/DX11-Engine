#pragma once

#include <algorithm>
#include <imgui.h>

namespace EditorUi {

inline float CompactControlWidth(
    float desiredWidth = 280.0f,
    float minWidth = 140.0f,
    float labelReserveWidth = 120.0f)
{
    const float availableWidth = ImGui::GetContentRegionAvail().x;
    const float expandedValueWidth = availableWidth - labelReserveWidth;
    return std::clamp(expandedValueWidth, minWidth, desiredWidth);
}

class ScopedItemWidth {
public:
    explicit ScopedItemWidth(
        float desiredWidth = 280.0f,
        float minWidth = 140.0f,
        float labelReserveWidth = 120.0f)
    {
        ImGui::PushItemWidth(CompactControlWidth(
            desiredWidth,
            minWidth,
            labelReserveWidth));
    }

    ~ScopedItemWidth()
    {
        ImGui::PopItemWidth();
    }

    ScopedItemWidth(const ScopedItemWidth&) = delete;
    ScopedItemWidth& operator=(const ScopedItemWidth&) = delete;
};

} // namespace EditorUi
