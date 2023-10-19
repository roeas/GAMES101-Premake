workspace("Frame")
	location(RootPath)
	targetdir(BinariesPath)
	
	architecture("x64")
	configurations { "Debug", "Release" }
	
	-- No optimization in debug mode.
	filter { "configurations:Debug" }
		symbols("On")
		optimize("Off")
		
	-- Full optimization in release maode.
	filter { "configurations:Release" }
		symbols("Off")
		optimize("Full")
		
	filter { "system:Windows" }
		systemversion("latest")
	filter {}
	
	startproject("Assignment0")
	