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
        "../Engine-Core/Source/Headers",
        "../Engine-Core/Source/cereal",
        "../Engine-Core/Source/imgui/include",
        "../Engine-Core/Source/glfw-3.3.8.bin.WIN64/include",
    }

    links
    {
        "Engine-Core",
        "../Engine-Core/Source/glfw-3.3.8.bin.WIN64/lib-vc2022/glfw3.lib",
        "../Engine-Core/Source/DirectXTex/lib/Release/DirectXTex.lib"
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

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"