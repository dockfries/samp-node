include(CheckTypeSize)
check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)

# Set this to false when using a custom local nodejs build for testing
set(__deps_check_enabled true)

# ── libnode version ────────────────────────────────────────────────
# PLUGIN_VERSION is defined in the root CMakeLists.txt and determines
# which Node.js version this build targets.

# ── libnode release source ─────────────────────────────────────────
# GitHub repo where libnode release assets are published.
# Override via: cmake -DNODE_REPO=your/repo
set(NODE_REPO "dockfries/libnode" CACHE STRING
    "GitHub repository for libnode releases")

# Tag prefix for the libnode release tag.
# When pushing tags: git tag v22.22.3  → prefix "v"
# When using manual dispatch: tag is libnode-v22.22.3  → prefix "libnode-v"
set(NODE_TAG_PREFIX "v" CACHE STRING
    "Prefix for the libnode release tag (e.g. v → v${PLUGIN_VERSION})")

# ── libnode library file names ─────────────────────────────────────
set(NODE_LIB_VERSION "127")
set(NODE_LIB_WIN "libnode.lib")
set(NODE_LIB_DLL "libnode.dll")
set(NODE_LIB_LINUX "libnode.so.${NODE_LIB_VERSION}")

function(download_file name path url)
    if(NOT EXISTS "${path}/${name}")
        message(STATUS "Downloading ${name}...")
        message(STATUS "  URL: ${url}")

        file(DOWNLOAD "${url}" "${path}/${name}"
            STATUS download_status
        )

        list(GET download_status 0 status_code)
        list(GET download_status 1 error_msg)

        if(NOT status_code EQUAL 0)
            message(FATAL_ERROR 
                "Failed to download '${url}' (code: ${status_code}): ${error_msg}"
            )
        endif()
        message(STATUS "Download ${name} - completed")
    endif()
endfunction()

function(get_os os_name)
    if(WIN32)
        set(os "win")
    else()
        set(os "linux")
    endif()
    set(${os_name} ${os} PARENT_SCOPE)
endfunction()

function(download_deps)
    if(NOT __deps_check_enabled)
        return()
    endif()

    set(base_path "${PROJECT_SOURCE_DIR}/deps/node/lib/Release")
    get_os(current_os)

    if(PLUGIN_ARCH STREQUAL "x64")
        set(arch_tag "64")
        set(arch_suffix "-x64")
    else()
        set(arch_tag "")
        set(arch_suffix "-x86")
    endif()

    set(target_path "${base_path}/${current_os}${arch_tag}")

    file(MAKE_DIRECTORY "${target_path}")

    # Construct download URL from repo + tag prefix + version + package name
    set(release_url "https://github.com/${NODE_REPO}/releases/download/${NODE_TAG_PREFIX}${PLUGIN_VERSION}")

    if(current_os STREQUAL "win")
        if(NOT EXISTS "${target_path}/${NODE_LIB_WIN}")
            message(STATUS "Downloading Windows ${PLUGIN_ARCH} dependencies...")
            set(pkg_name "libnode-windows${arch_suffix}.zip")
            set(pkg_path "${PROJECT_SOURCE_DIR}/deps/node/${pkg_name}")
            download_file(${pkg_name} "${PROJECT_SOURCE_DIR}/deps/node"
                "${release_url}/${pkg_name}")
            message(STATUS "Extracting ${pkg_name}...")
            file(ARCHIVE_EXTRACT INPUT "${pkg_path}" DESTINATION "${target_path}")
        endif()
    else()  # linux
        if(NOT EXISTS "${target_path}/${NODE_LIB_LINUX}")
            message(STATUS "Downloading Linux ${PLUGIN_ARCH} dependencies...")
            set(pkg_name "libnode-linux${arch_suffix}.tar.gz")
            set(pkg_path "${PROJECT_SOURCE_DIR}/deps/node/${pkg_name}")
            download_file(${pkg_name} "${PROJECT_SOURCE_DIR}/deps/node"
                "${release_url}/${pkg_name}")
            message(STATUS "Extracting ${pkg_name}...")
            file(ARCHIVE_EXTRACT INPUT "${pkg_path}" DESTINATION "${target_path}")
        endif()
    endif()
endfunction()
