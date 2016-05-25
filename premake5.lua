function CompilerSpecificFlags()
	filter { "action:vs*" }
		defines {
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NONSTDC_NO_DEPRECATE"
		}
	filter { "action:gmake" }
		buildoptions { "-std=c++0x" }
	filter {} -- to reset configuration filter
end

function RunTargetAfterBuild()
	configuration {"xcode*" }
		postbuildcommands {"$TARGET_BUILD_DIR/$TARGET_NAME"}

	configuration {"gmake"}
		postbuildcommands  { "$(TARGET)" }

	configuration {"codeblocks" }
		postbuildcommands { "$(TARGET_OUTPUT_FILE)"}

	configuration { "vs*"}
		postbuildcommands { "\"$(TargetPath)\"" }
	configuration "*" -- to reset configuration filter
end

solution "hiberlite"
	location "Build"

	platforms { "native", "x32", "x64" }
	configurations { "Debug", "Release" }

	objdir "obj/%{prj.name}/%{cfg.buildcfg}"
	targetdir "bin/%{prj.name}/%{cfg.buildcfg}"

	includedirs {
		[[./include]],
		[[./sqlite-amalgamation]],
		[[./Catch/single_include]]
	}

	vpaths {
		["Headers"] = {"**.h","**.hpp"},
		["Sources"] = {"**.c", "**.cpp"},
	}

	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		flags { "Symbols" }

	filter { "configurations:Release" }
		defines { "RELEASE" }
		flags { "Optimize" }

	filter {}

----------------------------------------------------------------------------------------------------------------
   project "hiberlite"
		language "C++"
		location "Build"
		kind "StaticLib"
		CompilerSpecificFlags()
		files {
			"./include/*.h",
			"./include/*.hpp",
			"./src/*.cpp",
		}

----------------------------------------------------------------------------------------------------------------
   project "sqlite"
   		language "C"
		kind "StaticLib"
		files {
			"./sqlite-amalgamation/*.h",
			"./sqlite-amalgamation/*.c",
		}

----------------------------------------------------------------------------------------------------------------

function default_project(name,files_table)
	project(name)
		language "C++"
		kind "ConsoleApp"
		files(files_table)
		links { "hiberlite", "sqlite" }
		CompilerSpecificFlags()
		RunTargetAfterBuild()

		filter {"system:linux"}
			links { "dl" , "pthread" }
		filter {}
end

default_project("sample",{ "./sample.cpp" })
default_project("catch_test",{ "./catch_tests.cpp" })
default_project("hiberlite_test",{ "./tests.cpp" })
