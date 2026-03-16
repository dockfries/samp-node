include(CheckTypeSize)
check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)

if(WIN32)
    if(NOT SIZEOF_VOID_P EQUAL 4)
        message(FATAL_ERROR "Only 32-bit builds are supported on Windows. Detected 64-bit.")
    endif()
elseif(NOT UNIX)
    message(FATAL_ERROR "Unsupported operating system.")
endif()

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
    set(target_path "${base_path}/${current_os}")
    
    file(MAKE_DIRECTORY "${target_path}")
    
    if(current_os STREQUAL "win")
        message(STATUS "Downloading Windows 32-bit dependencies...")
        download_file(${NODE_LIB_WIN} "${target_path}" ${NODE_LIB_WIN})
        download_file(${NODE_LIB_DLL} "${target_path}" ${NODE_LIB_DLL})
    else()  # linux
        message(STATUS "Downloading Linux 32-bit dependencies...")
        download_file(${NODE_LIB_LINUX} "${target_path}" ${NODE_LIB_LINUX})
    endif()
endfunction()
