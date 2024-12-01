include(CheckTypeSize)
check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)

function(DownloadFile name path urlpath)
    if(NOT EXISTS "${path}/${name}")
        message("Downloading ${name}...")

        set(__download_url "${__deps_url_base_path}/${urlpath}")

        file(DOWNLOAD "${__download_url}" "${path}/${name}"
            STATUS DOWNLOAD_STATUS
        )
        # Separate the returned status code, and error message.
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
        # Check if download was successful.
        if(${STATUS_CODE} EQUAL 0)
            message(STATUS "Download completed successfully!")
        else()
            # Exit CMake if the download failed, printing the error message.
            message(FATAL_ERROR "Error [${STATUS_CODE}] occurred during download '${__download_url}' : ${ERROR_MESSAGE}")
        endif()
    endif()
endfunction()

function(GetOS os)
    if(WIN32)
        set(__os_name "win")
    elseif(UNIX)
        set(__os_name "linux")
    endif()

    set(${os} ${__os_name} PARENT_SCOPE)
endfunction()

# Set this to false, when using a custom nodejs build for testing
set(__deps_check_enabled true)

function(DownloadDeps)
    set(__base_path "${PROJECT_SOURCE_DIR}/deps/node/lib/Release")

    GetOS(__deps_os_path_name)
    set(__deps_url_base_path "https://github.com/dockfries/samp-node/releases/download/lib20")

    if(__deps_check_enabled)
        if(WIN32)
            message("Checking windows binaries...")
            if(SIZEOF_VOID_P STREQUAL "4")
                DownloadFile("libnode.lib" "${__base_path}/${__deps_os_path_name}" "libnode.lib")
                DownloadFile("libnode.dll" "${__base_path}/${__deps_os_path_name}" "libnode.dll")
            else()
                message("Not Support x64...")
            endif()
        elseif(UNIX)
            message("Checking binaries...")

            if(SIZEOF_VOID_P STREQUAL "4")
                DownloadFile("libnode.so.115" "${__base_path}/${__deps_os_path_name}" "libnode.so.115")
            else()
                message("Not Support x64...")
            endif()
        endif()
    endif()
endfunction()