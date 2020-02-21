
include "Hazel"
hazelWorkspace("GroundStation")
setupHazel()

include "vendor/libusbp"

project "GroundStation"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/SLI-2020/Core/src/*.cpp",
	}

	includedirs
	{
		"src/",
	}

	sysincludedirs
	{
		"vendor/NeoGPS/src/",
		"vendor/EnumSerialPorts/",
		"vendor/SLI-2020/Core/include/",
	}

	defines
	{
		"GS_USE_PCH",
	}


	LibUSBPDependencies()
	HazelDependencies()

	filter "system:emscripten"
		linkoptions
		{
			"--preload-file assets"
		}

	filter "system:macosx or system:windows or system:linux"
		pchsource "src/gspch.cpp"
	
	filter "system:windows or system:linux"
		pchheader "gspch.h"

	filter "system:macosx"
		pchheader "src/gspch.h"--Why XCode


project "Test"
	location "Test"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"Test/src/**.h",
		"Test/src/**.cpp",
		"src/**.h",
		"src/**.cpp",
	}

	defines
	{
		"GROUND_STATION_TEST",
	}

	includedirs
	{
		"src/",
		"Test/src/",
	}

	sysincludedirs
	{
		"Test/vendor/",
		"vendor/NeoGPS/src/",
		"vendor/EnumSerialPorts/",
	}

	LibUSBPDependencies()
	HazelDependencies()


project "NeoGPS"
	kind "StaticLib"
	location "vendor/NeoGPS"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"vendor/NeoGPS/src/**.h",
		"vendor/NeoGPS/src/**.cpp"
	}

	includedirs
	{
		"vendor/NeoGPS/src/",
	}

