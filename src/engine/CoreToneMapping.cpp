#include "Core.h"
#include "vk_initializers.h"
#include "vk_images.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace Engine {

void Core::InitToneMappingPipeline()
{
    VkShaderModule shader = VK_NULL_HANDLE;
    if (!LoadEngineShaderModule("shaders/tone_mapping.comp.spv", &shader)) {
        throw std::runtime_error("Failed to load the final tone-mapping shader");
    }
    VkPushConstantRange pushRange{VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float)};
    auto layoutInfo = vkinit::pipeline_layout_create_info();
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &_drawImageDescriptorLayout;
    layoutInfo.pushConstantRangeCount = 1;
    layoutInfo.pPushConstantRanges = &pushRange;
    VK_CHECK(vkCreatePipelineLayout(_device, &layoutInfo, nullptr, &_toneMappingPipelineLayout));

    VkComputePipelineCreateInfo pipelineInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    pipelineInfo.layout = _toneMappingPipelineLayout;
    pipelineInfo.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineInfo.stage.module = shader;
    pipelineInfo.stage.pName = "main";
    const auto result = vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_toneMappingPipeline);
    vkDestroyShaderModule(_device, shader, nullptr);
    VK_CHECK(result);
    _mainDeletionQueue.push_function([this]() {
        vkDestroyPipeline(_device, _toneMappingPipeline, nullptr);
        vkDestroyPipelineLayout(_device, _toneMappingPipelineLayout, nullptr);
    });
}

void Core::DrawToneMapping(VkCommandBuffer cmd, float exposure)
{
    exposure = std::isfinite(exposure) ? std::clamp(exposure, 0.001f, 64.0f) : 1.0f;
    vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _toneMappingPipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _toneMappingPipelineLayout,
        0, 1, &_drawImageDescriptors, 0, nullptr);
    vkCmdPushConstants(cmd, _toneMappingPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float), &exposure);
    vkCmdDispatch(cmd, (_drawExtent.width + 15) / 16, (_drawExtent.height + 15) / 16, 1);
    vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

} // namespace Engine
