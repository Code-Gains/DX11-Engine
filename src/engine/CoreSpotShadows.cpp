#include "Core.h"
#include "SpotLighting.h"
#include "vk_images.h"
#include "vk_initializers.h"

namespace Engine {
void Core::InitSpotShadowResources()
{
    for (uint32_t i = 0; i < FRAME_OVERLAP; ++i) {
        auto& image = _frames[i]._spotShadowMap;
        image = CreateImage({SpotShadowResolution, SpotShadowResolution, 1}, VK_FORMAT_D32_SFLOAT,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
        ImmediateSubmit([&](VkCommandBuffer cmd) {
            const auto range = vkinit::image_subresource_range(VK_IMAGE_ASPECT_DEPTH_BIT);
            vkutil::transition_image(cmd, image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);
            VkClearDepthStencilValue clear{0.0f, 0};
            vkCmdClearDepthStencilImage(cmd, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear, 1, &range);
            vkutil::transition_image(cmd, image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL, range);
        });
        _mainDeletionQueue.push_function([this, i]() { DestroyImage(_frames[i]._spotShadowMap); });
    }
}

void Core::DrawSpotShadowMap(VkCommandBuffer cmd)
{
    const auto lights = CollectSpotLights(_registry);
    sceneData.spotLightCounts = glm::uvec4(lights.count, lights.eligibleCount, 0, 0);
    for (uint32_t i = 0; i < MaxSpotLights; ++i) sceneData.spotLights[i] = lights.lights[i];
    sceneData.spotShadowParameters.x = -1.0f;
    _spotShadowEntity = entt::null;
    _spotShadowDraws = 0;
    if (lights.shadowLightIndex < 0 || _shadowPipeline == VK_NULL_HANDLE) return;

    const auto& light = lights.lights[lights.shadowLightIndex];
    const auto position = glm::vec3(light.positionAndRange);
    const auto direction = glm::vec3(light.directionAndOuterCos);
    const auto farPlane = light.positionAndRange.w;
    const auto angle = glm::degrees(std::acos(light.directionAndOuterCos.w));
    sceneData.spotLightViewProjection = BuildSpotShadowMatrix(position, direction, angle, farPlane);
    sceneData.spotShadowParameters = glm::vec4(float(lights.shadowLightIndex), PointShadowNearPlane(farPlane), farPlane, lights.shadowBias);
    _spotShadowEntity = lights.shadowEntity;
    auto& frame = GetCurrentFrame();
    const auto range = vkinit::image_subresource_range(VK_IMAGE_ASPECT_DEPTH_BIT);
    vkutil::transition_image(cmd, frame._spotShadowMap.image, VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, range);
    auto attachment = vkinit::depth_attachment_info(frame._spotShadowMap.imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    auto rendering = vkinit::rendering_info({SpotShadowResolution, SpotShadowResolution}, nullptr, &attachment);
    vkCmdBeginRendering(cmd, &rendering);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _shadowPipeline);
    VkViewport viewport{0, 0, float(SpotShadowResolution), float(SpotShadowResolution), 0, 1};
    VkRect2D scissor{{0,0}, {SpotShadowResolution, SpotShadowResolution}};
    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
    size_t offset = 0;
    for (const auto& [mesh, instances] : _shadowBatches) {
        if (!mesh || mesh->surfaces.empty() || instances.empty()) continue;
        ShadowBatchPushConstants constants{};
        constants.lightViewProjection = sceneData.spotLightViewProjection;
        constants.vertexBuffer = mesh->meshBuffers.vertexBufferAddress;
        constants.instanceBuffer = frame._instanceBuffer.deviceAddress + offset;
        offset += instances.size() * sizeof(InstanceData);
        vkCmdPushConstants(cmd, _shadowPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(constants), &constants);
        vkCmdBindIndexBuffer(cmd, mesh->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
        for (uint32_t i = 0; i < instances.size(); ++i) {
            const auto& instance = instances[i];
            const auto scale = glm::abs(instance.scale);
            const auto center = instance.position + instance.rotation * (mesh->boundsCenter * instance.scale);
            const float radius = mesh->boundsRadius * std::max({scale.x, scale.y, scale.z});
            if (glm::length(center - position) > farPlane + radius ||
                !PointShadowSphereVisible(center, radius, sceneData.spotLightViewProjection)) continue;
            for (const auto& surface : mesh->surfaces) {
                vkCmdDrawIndexed(cmd, surface.count, 1, surface.startIndex, 0, i);
                ++_spotShadowDraws;
            }
        }
    }
    vkCmdEndRendering(cmd);
    vkutil::transition_image(cmd, frame._spotShadowMap.image, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL, range);
}
}
