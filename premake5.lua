-- premake5.lua
workspace "VkRayTracer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "VkRayTracer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "VkRayTracer"