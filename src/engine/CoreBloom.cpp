#include "Core.h"
#include "vk_images.h"
#include "vk_initializers.h"

#include <stdexcept>

namespace Engine {
namespace {
struct BloomPushConstants {
    glm::uvec4 sizes;
    glm::vec4 settings;
};
static_assert(sizeof(BloomPushConstants) == 32);
}

void Core::InitBloomPipelines()
{
    DescriptorLayoutBuilder builder;
    builder.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    builder.AddBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    _bloomDescriptorLayout = builder.Build(_device, VK_SHADER_STAGE_COMPUTE_BIT);
    VkPushConstantRange range{VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(BloomPushConstants)};
    auto layout = vkinit::pipeline_layout_create_info();
    layout.setLayoutCount = 1;
    layout.pSetLayouts = &_bloomDescriptorLayout;
    layout.pushConstantRangeCount = 1;
    layout.pPushConstantRanges = &range;
    VK_CHECK(vkCreatePipelineLayout(_device, &layout, nullptr, &_bloomPipelineLayout));

    const auto create = [&](const char* path, VkPipeline& pipeline) {
        VkShaderModule shader = VK_NULL_HANDLE;
        if (!LoadEngineShaderModule(path, &shader)) throw std::runtime_error("Failed to load bloom shader");
        VkComputePipelineCreateInfo info{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        info.layout = _bloomPipelineLayout;
        info.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
        info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        info.stage.module = shader;
        info.stage.pName = "main";
        const auto result = vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline);
        vkDestroyShaderModule(_device, shader, nullptr);
        VK_CHECK(result);
    };
    create("shaders/bloom_downsample.comp.spv", _bloomDownsamplePipeline);
    create("shaders/bloom_upsample.comp.spv", _bloomUpsamplePipeline);
    _mainDeletionQueue.push_function([this]() {
        for (auto& frame : _frames) {
            for (auto& image : frame._bloomImages) DestroyImage(image);
            frame._bloomImages.clear();
        }
        vkDestroyPipeline(_device, _bloomDownsamplePipeline, nullptr);
        vkDestroyPipeline(_device, _bloomUpsamplePipeline, nullptr);
        vkDestroyPipelineLayout(_device, _bloomPipelineLayout, nullptr);
        vkDestroyDescriptorSetLayout(_device, _bloomDescriptorLayout, nullptr);
    });
}

void Core::DrawBloom(VkCommandBuffer cmd, const PostProcessComponent* component)
{
    if (!component || !component->bloomEnabled) return;
    auto settings = *component;
    settings.Sanitize();
    if (settings.bloomIntensity == 0.0f || _drawExtent.width == 0 || _drawExtent.height == 0) return;

    auto& frame = GetCurrentFrame();
    if (frame._bloomExtent.width != _drawExtent.width || frame._bloomExtent.height != _drawExtent.height) {
        // This frame's fence has completed before command recording; other frames own separate images.
        for (auto& image : frame._bloomImages) DestroyImage(image);
        frame._bloomImages.clear();
        for (const auto extent : BuildBloomExtents(_drawExtent)) {
            frame._bloomImages.push_back(CreateImage({extent.width, extent.height, 1}, VK_FORMAT_R16G16B16A16_SFLOAT,
                VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT));
        }
        frame._bloomExtent = _drawExtent;
    }
    vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
    for (auto& image : frame._bloomImages)
        vkutil::transition_image(cmd, image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    const auto dispatch = [&](const AllocatedImage& source, VkExtent2D sourceExtent,
                              const AllocatedImage& destination, VkExtent2D destinationExtent,
                              VkPipeline pipeline, bool firstOrComposite) {
        auto descriptor = frame._frameDescriptors.allocate(_device, _bloomDescriptorLayout);
        DescriptorWriter writer;
        writer.write_image(0, source.imageView, _defaultSamplerLinear, VK_IMAGE_LAYOUT_GENERAL,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        writer.write_image(1, destination.imageView, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        writer.update_set(_device, descriptor);
        BloomPushConstants constants{
            {sourceExtent.width, sourceExtent.height, destinationExtent.width, destinationExtent.height},
            {settings.bloomThreshold, settings.bloomSoftKnee, settings.bloomIntensity, firstOrComposite ? 1.0f : 0.0f}};
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, _bloomPipelineLayout, 0, 1, &descriptor, 0, nullptr);
        vkCmdPushConstants(cmd, _bloomPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(constants), &constants);
        vkCmdDispatch(cmd, (destinationExtent.width + 7) / 8, (destinationExtent.height + 7) / 8, 1);
        vkutil::transition_image(cmd, destination.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL);
    };
    const auto extentOf = [](const AllocatedImage& image) {
        return VkExtent2D{image.imageExtent.width, image.imageExtent.height};
    };
    for (size_t i = 0; i < frame._bloomImages.size(); ++i) {
        const auto& source = i == 0 ? _drawImage : frame._bloomImages[i - 1];
        auto& destination = frame._bloomImages[i];
        dispatch(source, i == 0 ? _drawExtent : extentOf(source), destination, extentOf(destination),
            _bloomDownsamplePipeline, i == 0);
    }
    for (size_t i = frame._bloomImages.size() - 1; i > 0; --i) {
        const auto& source = frame._bloomImages[i];
        auto& destination = frame._bloomImages[i - 1];
        dispatch(source, extentOf(source), destination, extentOf(destination), _bloomUpsamplePipeline, false);
    }
    const auto& bloomImage = frame._bloomImages.front();
    dispatch(bloomImage, extentOf(bloomImage), _drawImage, _drawExtent, _bloomUpsamplePipeline, true);
    vkutil::transition_image(cmd, _drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}
}
