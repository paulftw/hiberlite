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
	language "C++"
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
	configuration {"gmake"}
		buildoptions { "-v -std=c++11 -fPIC" }
end

function CompilerSpecificPostBuildEvent()
	configuration {"xcode*" }
		postbuildcommands {"$TARGET_BUILD_DIR/$TARGET_NAME"}

	configuration {"gmake"}
		postbuildcommands  { "$(TARGET)" }

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
		files {
			"./include/*.h",
			"./include/*.hpp",
			"./src/*.cpp",
		}

----------------------------------------------------------------------------------------------------------------
   local sqlite=project "sqlite"
		kind "StaticLib"
		DefaultConfig()
		files {
			"./sqlite-amalgamation/*.h",
			"./sqlite-amalgamation/*.c",
		}

----------------------------------------------------------------------------------------------------------------

local make_console_project=function(name,input_files,post_build_event)
		project(name)
		kind "ConsoleApp"
		DefaultConfig()
		CompilerSpecificFlags()
		if type(post_build_event)=="function" then
			post_build_event()
		end
		language "C++"
		files ( input_files )
		links {
			"hiberlite",
			"sqlite"
		}
		configuration "linux"
			links { "dl" , "pthread" }			
end
----------------------------------------------------------------------------------------------------------------

make_console_project("sample",{ "./sample.cpp" },CompilerSpecificPostBuildEvent)
make_console_project("catch_test",{ "./catch_tests.cpp" },CompilerSpecificPostBuildEvent)
make_console_project("hiberlite_test",{ "./tests.cpp" },CompilerSpecificPostBuildEvent)
