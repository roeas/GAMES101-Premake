workspace("Frame")
	location(RootPath)
	
	architecture("x64")
	configurations { "Debug", "Release" }
	
	-- No optimization in Debug mode.
	filter { "configurations:Debug" }
		symbols("On")
		optimize("Off")
		
	-- Full optimization in Release maode.
	filter { "configurations:Release" }
		symbols("Off")
		optimize("Full")
		
	filter { "system:Windows" }
		systemversion("latest")
	filter {}
	
	startproject("Assignment0")
	