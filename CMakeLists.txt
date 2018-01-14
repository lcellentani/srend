cmake_minimum_required (VERSION 3.2 FATAL_ERROR)
project (srend VERSION 0.1 LANGUAGES CXX)

set (TARGET_NAME "srend")

set(CMAKE_SUPPRESS_REGENERATION true)
# explicitely specify which configurations we want
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# make all binary artifacts end up on the same place
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

include_directories("${PROJECT_SOURCE_DIR}/3rdparty/sdl/include")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/glm")

link_directories("${PROJECT_SOURCE_DIR}/3rdparty/sdl/lib/x86")

file(GLOB SREND_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/sources/*.h*")

list(APPEND SREND_ALL_INCLUDES ${SREND_INCLUDES}) 

add_executable(${TARGET_NAME}
	${SREND_ALL_INCLUDES}
	sources/events.cpp
	sources/display.cpp
    sources/main.cpp
	sources/rasterizer.cpp
	sources/timing.cpp
)

target_link_libraries(${TARGET_NAME}
    SDL2.lib
	SDL2main.lib
)

set_target_properties(${TARGET_NAME}
	PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/media"
)

if(MSVC)
	source_group("Header Files\\" FILES ${SREND_INCLUDES}) 

	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
	add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
	if (MSVC14)
		add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
	else()
		add_definitions(-D_HAS_ITERATOR_DEBUGGING=0)
	endif()
	
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
endif()

set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "/W4 /WX /GR- /Gy /arch:SSE2 /wd4201 /wd4324")

if(WIN32) # Check if we are on Windows
	if(MSVC) # Check if we are using the Visual Studio compiler
		set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
	elseif(CMAKE_COMPILER_IS_GNUCXX)
		# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
	else()
		message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
	endif()
elseif(UNIX)
	# Nothing special required
else()
	message(SEND_ERROR "You are on an unsupported platform! (Not Win32 or Unix)")
endif()

file(COPY ${PROJECT_SOURCE_DIR}/3rdparty/sdl/lib/x86/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR})
