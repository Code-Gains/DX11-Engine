#include "Core.h"
#include "Camera.h"
#include "SunlightComponent.h"
#include "stb_image.h"
#include <iostream>

int main(int argc, char** argv) {
    try {
        std::cout << std::unitbuf;
        const bool shadows = argc > 1 && std::string_view(argv[1]) == "shadows";
        const auto root = std::filesystem::path(ENGINE_TEST_ROOT).lexically_normal();
        const auto output = root / "build/spotlight-render-tests" / (shadows ? "shadows" : "unshadowed");
        std::filesystem::create_directories(output);
        Engine::Core core;
        core.SetEngineRoot(root);
        core.SetProjectRoot(output);
        core.SetBenchmarkOptions({true, 10, 30});
        core.Init();
        auto& registry = core.GetRegistry();
        const auto cameraEntity = registry.create();
        registry.emplace<Transform>(cameraEntity).position = {0,0,5};
        auto& camera = registry.emplace<Camera>(cameraEntity);
        camera.backgroundMode = CameraBackgroundMode::SolidColor;
        camera.clearColor = {0,0,0,1};
        camera.screenshotRequested = true;
        registry.emplace<ActiveCameraTag>(cameraEntity);
        const auto sun = registry.create();
        auto& sunlight = registry.emplace<SunlightComponent>(sun);
        sunlight.castShadows = false;
        sunlight.intensity = sunlight.ambient = sunlight.environmentDiffuseIntensity = sunlight.environmentSpecularIntensity = 0;

        const auto addQuad = [&](float size, glm::vec3 position, bool single) {
            std::vector<Vertex> vertices(4);
            vertices[0].position = {-size,-size,0};
            vertices[1].position = {size,-size,0};
            vertices[2].position = {size,size,0};
            vertices[3].position = {-size,size,0};
            for (auto& vertex : vertices) {
                vertex.normal = {0,0,1};
                vertex.tangent = {1,0,0,1};
            }
            std::vector<uint32_t> indices{0,1,2,0,2,3};
            auto mesh = core.CreateRuntimeMesh("Spotlight test quad", indices, vertices);
            mesh->surfaces[0].material = core.FindMaterial("Engine/DefaultPBR");
            const auto entity = registry.create();
            registry.emplace<Transform>(entity).position = position;
            registry.emplace<MeshComponent>(entity).mesh = mesh;
            if (single) registry.emplace<SingleRenderTag>(entity);
        };
        addQuad(3, {0,0,0}, false);
        addQuad(0.3f, {0,0,1}, true);
        const auto lightEntity = registry.create();
        auto& transform = registry.emplace<Transform>(lightEntity);
        transform.position = {1.5f,0,3};
        transform.rotation = glm::rotation(glm::vec3(0,0,-1), glm::normalize(-transform.position));
        auto& light = registry.emplace<SpotLightComponent>(lightEntity);
        light.intensity = 120;
        light.range = 10;
        light.outerAngle = 40;
        light.innerAngle = 30;
        light.castShadows = shadows;
        core.Run();

        const auto capture = output / "debug_capture/view.png";
        int width, height, channels;
        auto* pixels = stbi_load(capture.string().c_str(), &width, &height, &channels, 4);
        if (!pixels) throw std::runtime_error("Missing spotlight capture");
        size_t litPixels = 0;
        for (int i = 0; i < width * height; ++i) if (pixels[i * 4] > 30) ++litPixels;
        if (litPixels < 1000) throw std::runtime_error("Spotlight did not illuminate receiver");
        if (shadows) {
            int baselineWidth, baselineHeight;
            auto* baseline = stbi_load((root / "build/spotlight-render-tests/unshadowed/debug_capture/view.png").string().c_str(),
                &baselineWidth, &baselineHeight, &channels, 4);
            if (!baseline || baselineWidth != width || baselineHeight != height) throw std::runtime_error("Missing matching baseline");
            size_t shadowPixels = 0;
            for (int i = 0; i < width * height; ++i)
                if (baseline[i * 4] > pixels[i * 4] + 30) ++shadowPixels;
            stbi_image_free(baseline);
            std::cout << "Spotlight shadow pixels: " << shadowPixels << '\n';
            if (shadowPixels < 100) throw std::runtime_error("Spotlight did not cast a shadow");
        }
        stbi_image_free(pixels);
        std::cout << "Spotlight render test passed; lit pixels: " << litPixels << '\n';
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
