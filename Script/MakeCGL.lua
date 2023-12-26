projectName = "CGL"

currentPath = path.join(AssignmentsPath, projectName)
print("Generating "..projectName.."...")
print("Source path: "..currentPath)

project(projectName)
	kind("StaticLib")
	-- SharedLib, StaticLib, ConsoleApp, Utility
	language("C++")
	cppdialect("C++20")
	
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
	
	-- Set files.
	files {
		path.join(currentPath, "**.*"),
	}
	
	-- Set filter.
	vpaths {
		["Source/"] = { 
			path.join(currentPath, "**.*"),
		},
	}
	
	-- Set include paths.
	includedirs {
		ThirdPartyPath,
		path.join(ThirdPartyPath, "freetype/include"),
		path.join(ThirdPartyPath, "glew/include"),
		path.join(ThirdPartyPath, "glfw/include"),
	}
	
	-- Link to thirdparty libs.
	filter { "configurations:Debug" }
		libdirs {
			path.join(ThirdPartyPath, "freetype/build/Debug"),
			path.join(ThirdPartyPath, "glew/build/cmake/build/lib/Debug"),
			path.join(ThirdPartyPath, "glfw/build/src/Debug"),
		}
		links {
			"freetyped",
			"libglew32d",
			"glfw3",
			"opengl32",
		}
	filter { "configurations:Release" }
		libdirs {
			path.join(ThirdPartyPath, "freetype/build/Release"),
			path.join(ThirdPartyPath, "glew/build/cmake/build/lib/Release"),
			path.join(ThirdPartyPath, "glfw/build/src/Release"),
		}
		links {
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
	