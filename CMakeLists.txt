cmake_minimum_required(VERSION 3.19)
project(samp-node)

if (NOT PLUGIN_VERSION)
	set(PLUGIN_VERSION "22.22.1")
endif()

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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	set(CPACK_SYSTEM_NAME "linux")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so;.so.64")

	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)

	add_link_options(
		"-static-libgcc"
		"-static-libstdc++"
	)

	add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=1) # unofficial x86 should be zero for 20, 22 need be 1
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

add_subdirectory(src)
