#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include <vulkan/vulkan.h>

struct PostProcessComponent {
    bool bloomEnabled = true;
    float bloomIntensity = 0.15f;
    float bloomThreshold = 1.0f;
    float bloomSoftKnee = 0.5f;

    void Sanitize() {
        bloomIntensity = std::isfinite(bloomIntensity) ? std::clamp(bloomIntensity, 0.0f, 10.0f) : 0.15f;
        bloomThreshold = std::isfinite(bloomThreshold) ? std::clamp(bloomThreshold, 0.0f, 60000.0f) : 1.0f;
        bloomSoftKnee = std::isfinite(bloomSoftKnee) ? std::clamp(bloomSoftKnee, 0.0f, 1.0f) : 0.5f;
    }
};

inline std::vector<VkExtent2D> BuildBloomExtents(VkExtent2D extent) {
    std::vector<VkExtent2D> levels;
    if (extent.width == 0 || extent.height == 0) return levels;
    for (unsigned int i = 0; i < 6; ++i) {
        extent = {std::max(1u, (extent.width + 1) / 2), std::max(1u, (extent.height + 1) / 2)};
        levels.push_back(extent);
        if (extent.width == 1 && extent.height == 1) break;
    }
    return levels;
}
