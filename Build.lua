-- premake5.lua
workspace "Engine"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Engine-Editor"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Engine-Core"
	include "Engine-Core/Build-Core.lua"
group ""

include "Engine-Editor/Build-Editor.lua"
