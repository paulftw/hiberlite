local OS=os.get()

local cmd = {
 dir = { linux = "ls", windows = "dir" }
}

local Commands={}

for i,v in pairs(cmd) do
 Commands[i]=cmd[i][OS]
end

-- Apply to current "filter" (solution/project)
function DefaultConfig()
	location "Build"
	configuration "Debug"
		defines { "DEBUG", "_DEBUG" }
		objdir "Build/obj"
		flags { "Symbols" }
	configuration "Release"
		defines { "RELEASE" }
		objdir "Build/obj"
		flags { "Optimize" }
	configuration "*" -- to reset configuration filter
end

function CompilerSpecificFlags()
	configuration { "vs*"}
		defines {
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NONSTDC_NO_DEPRECATE"
		}
end

function CompilerSpecificPostBuildEvent()
	configuration {"xcode*" }
		postbuildcommands {"$TARGET_BUILD_DIR/$TARGET_NAME"}

	configuration {"gmake"}
		postbuildcommands  { "$(TARGET)" }
		buildoptions { "-v -std=gnu++0x -fPIC" }

	configuration {"codeblocks" }
		postbuildcommands { "$(TARGET_OUTPUT_FILE)"}

	configuration { "vs*"}
		postbuildcommands { "\"$(TargetPath)\"" }
end

----------------------------------------------------------------------------------------------------------------

-- A solution contains projects, and defines the available configurations
local sln=solution "hiberlite"
	location "Build"
		sln.absbasedir=path.getabsolute(sln.basedir)
		configurations { "Debug", "Release" }
		platforms { "native","x32", "x64" }
		libdirs { }
		includedirs {
			[[./include]],
			[[./sqlite-amalgamation]],
			[[./Catch/single_include]]
		}
		vpaths {
			["Headers"] = {"**.h","**.hpp"},
			["Sources"] = {"**.c", "**.cpp"},
		}

----------------------------------------------------------------------------------------------------------------
   local hiberlite=project "hiberlite"
   location "Build"
		kind "StaticLib"
		DefaultConfig()
		CompilerSpecificFlags()
		language "C++"
		files {
			"./include/*.h",
			"./include/*.hpp",
			"./src/*.cpp",
		}

----------------------------------------------------------------------------------------------------------------
   local hiberlite=project "sqlite"
   location "Build"
		kind "StaticLib"
		DefaultConfig()
		language "C++"
		files {
			"./sqlite-amalgamation/*.h",
			"./sqlite-amalgamation/*.c",
		}

----------------------------------------------------------------------------------------------------------------

local function ConsoleApp(name,file_list,postbuild_event)
   local hiberlite_test=project(name)
   location "Build"
		kind "ConsoleApp"
		DefaultConfig()
		CompilerSpecificFlags()
		
		if type(postbuild_event)=="function" then
			postbuild_event()
		end
		
		language "C++"
		
		files (file_list)
		
		links {
			"sqlite",
			"hiberlite"
		}
		
		configuration "linux"
			links { }
		configuration "windows"
			links { }
end

----------------------------------------------------------------------------------------------------------------

ConsoleApp("hiberlite_test",{ "./tests.cpp" },CompilerSpecificPostBuildEvent)
ConsoleApp("catch_test",{ "./catch_tests.cpp" },CompilerSpecificPostBuildEvent)
ConsoleApp("sample",{ "./sample.cpp" })
