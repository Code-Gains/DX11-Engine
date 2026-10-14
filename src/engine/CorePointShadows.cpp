#include "Core.h"
#include "PointLighting.h"
#include "PointShadowMath.h"
#include "vk_images.h"
#include "vk_initializers.h"

namespace Engine {

void Core::InitPointShadowResources()
{
    VkSamplerCreateInfo sampler{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    sampler.magFilter = VK_FILTER_NEAREST;
    sampler.minFilter = VK_FILTER_NEAREST;
    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    sampler.addressModeU = sampler.addressModeV = sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler.maxLod = 0.0f;
    VK_CHECK(vkCreateSampler(_device, &sampler, nullptr, &_pointShadowSampler));
    _mainDeletionQueue.push_function([this]() { vkDestroySampler(_device, _pointShadowSampler, nullptr); });

    for (unsigned int frameIndex = 0; frameIndex < FRAME_OVERLAP; ++frameIndex) {
        auto& frame = _frames[frameIndex];
        auto& image = frame._pointShadowMap;
        image.imageExtent = {PointShadowResolution, PointShadowResolution, 1};
        image.imageFormat = VK_FORMAT_D32_SFLOAT;
        VkImageCreateInfo info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
        info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.format = image.imageFormat;
        info.extent = image.imageExtent;
        info.mipLevels = 1;
        info.arrayLayers = 6;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        VmaAllocationCreateInfo allocation{};
        allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        VK_CHECK(vmaCreateImage(_allocator, &info, &allocation, &image.image, &image.allocation, nullptr));

        VkImageViewCreateInfo view{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view.image = image.image;
        view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        view.format = image.imageFormat;
        view.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 6};
        VK_CHECK(vkCreateImageView(_device, &view, nullptr, &image.imageView));
        view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view.subresourceRange.layerCount = 1;
        for (uint32_t face = 0; face < 6; ++face) {
            view.subresourceRange.baseArrayLayer = face;
            VK_CHECK(vkCreateImageView(_device, &view, nullptr, &frame._pointShadowFaces[face]));
        }

        // Even an unused shadow descriptor always references initialized, readable depth.
        ImmediateSubmit([&](VkCommandBuffer cmd) {
            VkImageSubresourceRange range{VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 6};
            vkutil::transition_image(cmd, image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);
            VkClearDepthStencilValue clear{0.0f, 0};
            vkCmdClearDepthStencilImage(cmd, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear, 1, &range);
            vkutil::transition_image(cmd, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL, range);
        });

        frame._pointShadowDescriptorSet = globalDescriptorAllocator.Allocate(_device, _shadowDescriptorLayout);
        DescriptorWriter writer;
        writer.write_image(0, _shadowMapImage.imageView, _shadowMapSampler, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        writer.write_image(1, image.imageView, _pointShadowSampler, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        writer.write_image(2, frame._spotShadowMap.imageView, _pointShadowSampler, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        writer.update_set(_device, frame._pointShadowDescriptorSet);

        _mainDeletionQueue.push_function([this, frameIndex]() {
            auto& frame = _frames[frameIndex];
            for (auto view : frame._pointShadowFaces) vkDestroyImageView(_device, view, nullptr);
            DestroyImage(frame._pointShadowMap);
        });
    }
}

void Core::DrawPointShadowMap(VkCommandBuffer cmd)
{
    const auto lights = CollectPointLights(_registry);
    sceneData.pointLightCounts = glm::uvec4(_pointLightingEnabled ? lights.count : 0, lights.eligibleCount, 0, 0);
    for (uint32_t i = 0; i < MaxPointLights; ++i) sceneData.pointLights[i] = lights.lights[i];
    sceneData.pointShadowParameters.x = -1.0f;
    _pointShadowEntity = entt::null;
    _pointShadowDraws = 0;
    if (!_pointLightingEnabled || lights.shadowLightIndex < 0 || _shadowPipeline == VK_NULL_HANDLE) return;

    const auto& light = lights.lights[lights.shadowLightIndex];
    const auto position = glm::vec3(light.positionAndRange);
    const float farPlane = light.positionAndRange.w;
    const float nearPlane = PointShadowNearPlane(farPlane);
    sceneData.pointShadowParameters = glm::vec4(float(lights.shadowLightIndex), nearPlane, farPlane, lights.shadowBias);
    _pointShadowEntity = lights.shadowEntity;
    const auto matrices = BuildPointShadowMatrices(position, nearPlane, farPlane);
    auto& frame = GetCurrentFrame();
    VkImageSubresourceRange range{VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 6};
    vkutil::transition_image(cmd, frame._pointShadowMap.image, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, range);
    VkViewport viewport{0.0f, 0.0f, float(PointShadowResolution), float(PointShadowResolution), 0.0f, 1.0f};
    VkRect2D scissor{{0, 0}, {PointShadowResolution, PointShadowResolution}};
    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    for (uint32_t face = 0; face < 6; ++face) {
        auto attachment = vkinit::depth_attachment_info(frame._pointShadowFaces[face], VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
        auto rendering = vkinit::rendering_info({PointShadowResolution, PointShadowResolution}, nullptr, &attachment);
        vkCmdBeginRendering(cmd, &rendering);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _shadowPipeline);
        size_t offset = 0;
        for (const auto& [mesh, instances] : _shadowBatches) {
            if (!mesh || mesh->surfaces.empty() || instances.empty()) continue;
            ShadowBatchPushConstants constants{};
            constants.lightViewProjection = matrices[face];
            constants.vertexBuffer = mesh->meshBuffers.vertexBufferAddress;
            // Reuse the immutable instance upload made by DrawShadowMap in the same batch order.
            constants.instanceBuffer = frame._instanceBuffer.deviceAddress + offset;
            offset += instances.size() * sizeof(InstanceData);
            vkCmdPushConstants(cmd, _shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(constants), &constants);
            vkCmdBindIndexBuffer(cmd, mesh->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
            for (uint32_t i = 0; i < instances.size(); ++i) {
                const auto& instance = instances[i];
                const auto scale = glm::abs(instance.scale);
                const float radius = mesh->boundsRadius * std::max({scale.x, scale.y, scale.z});
                const auto center = instance.position + instance.rotation * (mesh->boundsCenter * instance.scale);
                if (glm::length(center - position) > farPlane + radius ||
                    !PointShadowSphereVisible(center, radius, matrices[face])) continue;
                for (const auto& surface : mesh->surfaces) {
                    vkCmdDrawIndexed(cmd, surface.count, 1, surface.startIndex, 0, i);
                    ++_pointShadowDraws;
                }
            }
        }
        vkCmdEndRendering(cmd);
    }
    vkutil::transition_image(cmd, frame._pointShadowMap.image, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL, range);
}
}
