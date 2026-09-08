cmake_minimum_required(VERSION 3.19)
project(samp-node)

if (NOT PLUGIN_VERSION)
	set(PLUGIN_VERSION "22.23.2")
endif()

set(PLUGIN_ARCH "x86" CACHE STRING "Target architecture: x86 (32-bit) or x64 (64-bit)")
set_property(CACHE PLUGIN_ARCH PROPERTY STRINGS x86 x64)

include(./DepsDownload.cmake)

download_deps()

# -
# Paths
# -

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test/plugins)
set(CMAKE_INSTALL_PREFIX ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(CMAKE_BUILD_WITH_INSTALL_RPATH true)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/deps/cmake-modules")

# -
# Compiler Flags
# -

if(UNIX)
	if(PLUGIN_ARCH STREQUAL "x86")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
		set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} --32")
		set(CPACK_SYSTEM_NAME "linux")
		set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so;.so.64")
		set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
		add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=1) # unofficial x86 should be zero for 20, 22 need be 1
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
		set(CPACK_SYSTEM_NAME "linux64")
		set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so")
		set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)
	endif()

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

# sampgdk is consumed as a git submodule. Match its arch to PLUGIN_ARCH and
# build it as a static, tiny library linked into the samp-node plugin module.
if(PLUGIN_ARCH STREQUAL "x64")
	set(SAMPGDK_ARCH "64" CACHE STRING "" FORCE)
else()
	set(SAMPGDK_ARCH "32" CACHE STRING "" FORCE)
endif()
set(SAMPGDK_STATIC ON CACHE BOOL "" FORCE)
set(SAMPGDK_TINY ON CACHE BOOL "" FORCE)
set(SAMPGDK_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(SAMPGDK_BUILD_PLUGINS OFF CACHE BOOL "" FORCE)
set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/deps/samp-plugin-sdk")

add_subdirectory(deps/sampgdk sampgdk)
add_subdirectory(src)
