#include "Core.h"
#include "Camera.h"
#include "MeshComponent.h"
#include "stb_image.h"

#include <iostream>
#ifdef _WIN32
#include <crtdbg.h>
#endif

int RunRenderTest(int argc, char** argv)
{
    std::cout << std::unitbuf;
#ifdef _WIN32
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif
    std::cout << "Starting bloom render test\n";
    const bool enabled = argc > 1 && std::string_view(argv[1]) == "enabled";
    const auto root = std::filesystem::path(ENGINE_TEST_ROOT).lexically_normal();
    const auto output = root / "build/bloom-render-tests" / (enabled ? "enabled" : "disabled");
    std::filesystem::create_directories(output);
    Engine::Core core;
    std::cout << "Setting test roots\n";
    core.SetEngineRoot(root);
    core.SetProjectRoot(output);
    core.SetBenchmarkOptions({true, 30, 60});
    std::cout << "Initializing renderer\n";
    core.Init();
    auto& registry = core.GetRegistry();
    const auto cameraEntity = registry.create();
    registry.emplace<Transform>(cameraEntity).position = {0.0f, 0.0f, 5.0f};
    auto& camera = registry.emplace<Camera>(cameraEntity);
    camera.backgroundMode = CameraBackgroundMode::SolidColor;
    camera.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    camera.screenshotRequested = true;
    registry.emplace<ActiveCameraTag>(cameraEntity);
    auto& post = registry.emplace<PostProcessComponent>(cameraEntity);
    post.bloomEnabled = enabled;
    post.bloomIntensity = 0.5f;

    std::vector<Vertex> vertices(3);
    vertices[0].position = {-0.5f, -0.5f, 0.0f};
    vertices[1].position = {0.5f, -0.5f, 0.0f};
    vertices[2].position = {0.0f, 0.5f, 0.0f};
    for (auto& vertex : vertices) {
        vertex.normal = {0.0f, 0.0f, 1.0f};
        vertex.tangent = {1.0f, 0.0f, 0.0f, 1.0f};
    }
    std::vector<uint32_t> indices{0, 1, 2};
    auto mesh = core.CreateRuntimeMesh("Bloom test emitter", indices, vertices);
    mesh->surfaces[0].material = core.FindMaterial("Engine/DefaultPBR");
    const auto emitter = registry.create();
    registry.emplace<Transform>(emitter);
    auto& component = registry.emplace<MeshComponent>(emitter);
    component.mesh = mesh;
    component.emissiveColor = {1.0f, 0.3f, 0.05f};
    component.emissiveStrength = 10.0f;
    core.Run();

    const auto capture = output / "debug_capture/view.png";
    int width, height, channels;
    auto* pixels = stbi_load(capture.string().c_str(), &width, &height, &channels, 4);
    if (!pixels) {
        std::cerr << "Missing bloom render capture: " << capture << '\n';
        return 1;
    }
    size_t litPixels = 0;
    for (int i = 0; i < width * height; ++i)
        if (pixels[i * 4] > 4 || pixels[i * 4 + 1] > 4 || pixels[i * 4 + 2] > 4) ++litPixels;
    stbi_image_free(pixels);
    std::cout << "Bloom " << (enabled ? "enabled" : "disabled") << ": lit pixels=" << litPixels << '\n';
    if (litPixels < 100) return 1;
    if (enabled) {
        const auto baseline = root / "build/bloom-render-tests/disabled/debug_capture/view.png";
        auto* original = stbi_load(baseline.string().c_str(), &width, &height, &channels, 4);
        if (!original) return 1;
        size_t baselinePixels = 0;
        for (int i = 0; i < width * height; ++i)
            if (original[i * 4] > 4 || original[i * 4 + 1] > 4 || original[i * 4 + 2] > 4) ++baselinePixels;
        stbi_image_free(original);
        if (litPixels <= baselinePixels + 100) {
            std::cerr << "Bloom did not spread beyond the emitter\n";
            return 1;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    try {
        return RunRenderTest(argc, argv);
    } catch (const std::exception& error) {
        std::cerr << "Bloom render test failed: " << error.what() << '\n';
        return 1;
    }
}
