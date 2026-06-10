include(CheckTypeSize)
check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)

# Set this to false, when using a custom nodejs build for testing
set(__deps_check_enabled true)

set(NODE_LIB_VERSION "127")
set(NODE_LIB_WIN "libnode.lib")
set(NODE_LIB_DLL "libnode.dll")
set(NODE_LIB_LINUX "libnode.so.${NODE_LIB_VERSION}")
set(DEPS_URL_BASE "https://github.com/dockfries/samp-node/releases/download/${PLUGIN_VERSION}")

function(download_file name path url_path)
    if(NOT EXISTS "${path}/${name}")
        message(STATUS "Downloading ${name}...")
        
        set(full_url "${DEPS_URL_BASE}/${url_path}")
        file(DOWNLOAD "${full_url}" "${path}/${name}"
            STATUS download_status
        )
        
        list(GET download_status 0 status_code)
        list(GET download_status 1 error_msg)
        
        if(NOT status_code EQUAL 0)
            message(FATAL_ERROR 
                "Failed to download '${full_url}' (code: ${status_code}): ${error_msg}"
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
    
    if(current_os STREQUAL "win")
        if(NOT EXISTS "${target_path}/${NODE_LIB_WIN}")
            message(STATUS "Downloading Windows ${PLUGIN_ARCH} dependencies...")
            set(pkg_name "libnode-windows${arch_suffix}.zip")
            set(pkg_path "${PROJECT_SOURCE_DIR}/deps/node/${pkg_name}")
            download_file(${pkg_name} "${PROJECT_SOURCE_DIR}/deps/node" ${pkg_name})
            message(STATUS "Extracting ${pkg_name}...")
            file(ARCHIVE_EXTRACT INPUT "${pkg_path}" DESTINATION "${target_path}")
        endif()
    else()  # linux
        if(NOT EXISTS "${target_path}/${NODE_LIB_LINUX}")
            message(STATUS "Downloading Linux ${PLUGIN_ARCH} dependencies...")
            set(pkg_name "libnode-linux${arch_suffix}.tar.gz")
            set(pkg_path "${PROJECT_SOURCE_DIR}/deps/node/${pkg_name}")
            download_file(${pkg_name} "${PROJECT_SOURCE_DIR}/deps/node" ${pkg_name})
            message(STATUS "Extracting ${pkg_name}...")
            file(ARCHIVE_EXTRACT INPUT "${pkg_path}" DESTINATION "${target_path}")
        endif()
    endif()
endfunction()
