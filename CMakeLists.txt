cmake_minimum_required(VERSION 3.19)
project(samp-node)

if (NOT PLUGIN_VERSION)
	set(PLUGIN_VERSION "24.21.0")
endif()

include(./DepsDownload.cmake)

download_deps()

# -
# Paths
# -

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test/plugins)
set(CMAKE_INSTALL_PREFIX ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(CMAKE_BUILD_WITH_INSTALL_RPATH true)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/deps/cmake-modules")

# -
# Compiler Flags
# - 

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20 /Zc:__cplusplus")
endif()

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
	set(CPACK_SYSTEM_NAME "linux64")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)

	add_link_options(
		"-static-libgcc"
		"-static-libstdc++"
	)
endif()

if(WIN32)
	add_definitions(
		-D_CRT_SECURE_NO_WARNINGS
		-DWIN32_LEAN_AND_MEAN
	)	
endif()

# -
# Directories
# -

# sampgdk is consumed as a git submodule. node-24 targets 64-bit only; build
# sampgdk as a static, tiny library linked into the samp-node plugin module.
set(SAMPGDK_ARCH "64" CACHE STRING "" FORCE)
set(SAMPGDK_STATIC ON CACHE BOOL "" FORCE)
set(SAMPGDK_TINY ON CACHE BOOL "" FORCE)
set(SAMPGDK_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(SAMPGDK_BUILD_PLUGINS OFF CACHE BOOL "" FORCE)
set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/deps/samp-plugin-sdk")

add_subdirectory(deps/sampgdk sampgdk)
add_subdirectory(src)
