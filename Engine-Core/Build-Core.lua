project "Engine-Core"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.hpp", "Source/**.cpp" }

   includedirs
   {
      "Source",
      "Source/include",
      "Source/cereal",
      "Source/imgui/include",
      "Source/glfw-3.3.8.bin.WIN64/include",
      "Source/FreeImage/include",
      "%{vulkanSDK}/Include",
      "Source/Engine/Graphics", 
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { }

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
