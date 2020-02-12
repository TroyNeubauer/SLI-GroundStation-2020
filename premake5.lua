
include "Hazel"
hazelWorkspace("GroundStation")
setupHazel()

project "GroundStation"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src/",
		"vendor/NeoGPS/src",
	}

	HazelDependencies()

	filter "system:emscripten"
		linkoptions
		{
			"--preload-file assets"
		}

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
	}

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