project "Engine-Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    files { "Source/**.h", "Source/**.hpp", "Source/**.cpp" }

    includedirs
    {
        "Source",

        -- Include Core
        "../Engine-Core/Source",
        "../Engine-Core/Source/include",
        "../Engine-Core/Source/cereal",
        "../Engine-Core/Source/imgui/include",
        "../Engine-Core/Source/glfw-3.3.8.bin.WIN64/include",
        "../Engine-Core/Source/FreeImage/include",
        "%{vulkanSDK}/Include" -- Add Vulkan include directory
    }

    links
    {
        "Engine-Core",
        "../Engine-Core/Source/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.lib",
        "../Engine-Core/Source/DirectXTex/lib/Release/DirectXTex.lib",
        "../Engine-Core/Source/FreeImage/lib//FreeImage.lib",
        "vulkan-1" -- Link Vulkan library
    }

    postbuildcommands {
        '{COPY} "%{wks.location}/Assets/Shaders/*" "%{cfg.targetdir}/Assets/Shaders/"'
    }

    debugdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
    targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"
        defines { "WINDOWS" }

    filter "configurations:Debug_DX11"
        defines { "DEBUG", "USE_DIRECTX11" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release_DX11"
        defines { "RELEASE", "USE_DIRECTX11" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist_DX11"
        defines { "DIST", "USE_DIRECTX11" }
        runtime "Release"
        optimize "On"
        symbols "Off"

    filter "configurations:Debug_Vulkan"
        defines { "DEBUG", "USE_VULKAN" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release_Vulkan"
        defines { "RELEASE", "USE_VULKAN" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist_Vulkan"
        defines { "DIST", "USE_VULKAN" }
        runtime "Release"
        optimize "On"
        symbols "Off"
