projectName = "Assignment8"

assignmentPath = path.join(AssignmentsPath, projectName)
print("Generating "..projectName.."...")
print("Source path: "..assignmentPath)

project(projectName)
	kind("ConsoleApp")
	-- SharedLib, StaticLib, ConsoleApp
	language("C++")
	cppdialect("C++20")
	dependson("CGL")
	
	-- Intermediate and binary path.
	location(IntermediatePath)
	objdir(path.join(IntermediatePath, "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"))
	targetdir(path.join(BinaryPath, "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"))
	
	-- Target name.
	filter { "configurations:Debug" }
		targetname("%{prj.name}".."d")
	filter { "configurations:Release" }
		targetname("%{prj.name}")
	filter {}
	
	-- Set definitions.
	defines {
		"FRAME_ASSET_PATH=\""..AssetPath.."\"",
	}
	
	-- Set files.
	files {
		path.join(AssignmentsPath, "Utils.hpp"),
		path.join(assignmentPath, "**.*"),
	}
	
	-- Set filter.
	vpaths {
		["Source/Utils/"] = { 
			path.join(AssignmentsPath, "Utils.hpp"),
		},
		["Source/"] = { 
			path.join(assignmentPath, "**.*"),
		},
	}
	
	-- Set include paths.
	includedirs {
		AssignmentsPath,
		ThirdPartyPath,
		path.join(ThirdPartyPath, "freetype/include"),
		path.join(ThirdPartyPath, "glew/include"),
		path.join(ThirdPartyPath, "glfw/include"),
	}
	
	-- Link to thirdparty libs.
	filter { "configurations:Debug" }
		libdirs {
			path.join(BinaryPath, "Debug-windows-x86_64/CGL"),
			path.join(ThirdPartyPath, "freetype/build/Debug"),
			path.join(ThirdPartyPath, "glew/build/cmake/build/lib/Debug"),
			path.join(ThirdPartyPath, "glfw/build/src/Debug"),
		}
		links {
			"CGLd",
			"freetyped",
			"libglew32d",
			"glfw3",
			"opengl32",
		}
	filter { "configurations:Release" }
		libdirs {
			path.join(BinaryPath, "Release-windows-x86_64/CGL"),
			path.join(ThirdPartyPath, "freetype/build/Release"),
			path.join(ThirdPartyPath, "glew/build/cmake/build/lib/Release"),
			path.join(ThirdPartyPath, "glfw/build/src/Release"),
		}
		links {
			"CGL",
			"freetype",
			"libglew32",
			"glfw3",
			"opengl32",
		}
	filter {}
	
	defines {
		"GLEW_STATIC",
	}
	
	-- Use /MT and /MTd.
	staticruntime "on"
	filter { "configurations:Debug" }
		runtime("Debug") -- /MTd
	filter { "configurations:Release" }
		runtime("Release") -- /MT
	filter {}
	
	-- Disable these options can reduce the size of compiled binaries.
	justmycode("Off")
	editAndContinue("Off")
	exceptionhandling("Off")
	rtti("Off")
	
	-- Strict.
	warnings("Default")
	externalwarnings("Off")
	
	flags {
		-- Compiler uses multiple thread.
		"MultiProcessorCompile",
	}
	