--

solution "srend"
	configurations {
		"Debug",
		"Release",
	}

	platforms {
		"x32",
		"x64",
--		"Xbox360",
		"Native", -- for targets where bitness is not specified
	}

	language "C++"
	startproject "srend"

MODULE_DIR = path.getabsolute("../")
SREND_DIR = path.getabsolute("..")

local SREND_BUILD_DIR = path.join(SREND_DIR, ".build")
local SREND_THIRD_PARTY_DIR = path.join(SREND_DIR, "3rdparty")

dofile (path.join(SREND_DIR, "scripts/toolchain.lua"))
if not toolchain(SREND_BUILD_DIR, SREND_THIRD_PARTY_DIR) then
	return -- no action specified
end

function copyLib()
end

function mainProject(_name)

	project (_name)
		uuid (os.uuid(_name))
		kind "WindowedApp"

	configuration {}

	debugdir (path.join(SREND_DIR, "media/"))

	includedirs {
--		path.join(SREND_DIR, "3rdparty/stb"),
		path.join(SREND_DIR, "sources/"),
	}

	files {
		path.join(SREND_DIR, "sources/", "**.c"),
		path.join(SREND_DIR, "sources/", "**.cpp"),
		path.join(SREND_DIR, "sources/", "**.h"),
	}

	removefiles {
		path.join(SREND_DIR, "sources/", "**.bin.h"),
	}
	
	configuration { "x32", "vs*" }
		libdirs {
			path.join(SREND_BUILD_DIR, "win32_" .. _ACTION, "bin"),
		}

	configuration { "x64", "vs*" }
		libdirs {
			path.join(SREND_BUILD_DIR, "win64_" .. _ACTION, "bin"),
		}
		
	configuration { "vs*", "x32 or x64" }
		linkoptions {
			"/ignore:4199", -- LNK4199: /DELAYLOAD:*.dll ignored; no imports found from *.dll
		}
		
	configuration {}

	strip()
end

mainProject("srend")


