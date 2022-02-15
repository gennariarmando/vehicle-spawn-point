workspace "vehicle-spawn-point"
	configurations { "Release", "Debug" }
	location "project_files"
   
project "vehicle-spawn-point"
	files {
		"source/**.*"
	}
	
	includedirs { 
		"source/**",
	}
	
	includedirs {
		"$(PLUGIN_SDK_DIR)/shared/",
		"$(PLUGIN_SDK_DIR)/shared/game/",
		"$(PLUGIN_SDK_DIR)/plugin_vc/",
		"$(PLUGIN_SDK_DIR)/plugin_vc/game_vc/",
	}
	
	libdirs { 
		"$(PLUGIN_SDK_DIR)/output/lib/",
	}

	kind "SharedLib"
	language "C++"
	targetdir "output/asi/"
	objdir ("output/obj")
	targetextension ".asi"
	characterset ("MBCS")
	linkoptions "/SAFESEH:NO"
	buildoptions { "-std:c++latest", "/permissive" }
	defines { "_CRT_SECURE_NO_WARNINGS", "_CRT_NON_CONFORMING_SWPRINTFS", "_USE_MATH_DEFINES" }
    disablewarnings { "4244", "4800", "4305", "6031"}
	defines { "GTAVC", "PLUGIN_SGV_10EN" }

	filter "configurations:Debug"		
		links { "plugin_vc_d" }
		targetname "VehicleSpawnPoint"
		defines { "DEBUG" }
		symbols "on"
		staticruntime "on"
		debugdir "$(GTA_VC_DIR)"
		debugcommand "$(GTA_VC_DIR)/gta-vc.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"$(GTA_VC_DIR)\\scripts\\VehicleSpawnPoint.asi\""

	filter "configurations:Release"
		links { "plugin_vc" }
		targetname "VehicleSpawnPoint"
		defines { "NDEBUG" }
		symbols "off"
		optimize "On"
		staticruntime "on"
		debugdir "$(GTA_VC_DIR)"
		debugcommand "$(GTA_VC_DIR)/gta-vc.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"$(GTA_VC_DIR)\\scripts\\VehicleSpawnPoint.asi\""
