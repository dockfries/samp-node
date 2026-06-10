/* Copyright (C) 2011-2018 Zeex
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SAMPGDK_BOOL_H
#define SAMPGDK_BOOL_H

/* bool */
#if !defined __cplusplus && !defined HAVE_BOOL
  /* If HAVE_BOOL is not defined we attempt to detect stdbool.h first,
   * then define our own "bool" type.
   */
  #if defined __STDC__ && defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L\
      || defined HAVE_STDBOOL_H
    /* Have a C99-conformant compiler. */
    #include <stdbool.h>
  #else
    typedef unsigned char bool;
    #define true 1
    #define false 0
    #define __bool_true_false_are_defined
  #endif
#else
  /* Make sure their "bool" is one byte in size. This is required for binary
   * compatibility with C++ code. */
  typedef int sizeof_bool_must_be_1[sizeof(bool) == 1 ? 1 : -1];
#endif

#endif /* !SAMPGDK_BOOL_H */

#ifndef SAMPGDK_PLATFORM_H
#define SAMPGDK_PLATFORM_H

#if !defined _M_IX86 && !defined __i386__ \
    && !defined _M_X64 && !defined __x86_64__ \
    && !defined RC_INVOKED
  #error Unsupported architecture
#endif

#if defined _M_X64 || defined __x86_64__ || defined __64BIT__
  #define SAMPGDK_64BIT 1
#endif

#if (defined __CYGWIN32__ || defined RC_INVOKED) && !defined WIN32
  #define WIN32
#endif

#if defined WIN32 || defined _WIN32 || defined __WIN32__
  #define SAMPGDK_LINUX 0
  #define SAMPGDK_WINDOWS 1
#endif

#if defined __linux__ || defined __linux || defined linux
  #if !defined LINUX
    #define LINUX
  #endif
  #define SAMPGDK_LINUX 1
  #define SAMPGDK_WINDOWS 0
#endif

#if defined __GNUC__
  #define SAMPGDK_DEPRECATED_API(type, rest) \
    SAMPGDK_API(type, rest) __attribute__((deprecated))
#elif defined _MSC_VER
  #define SAMPGDK_DEPRECATED_API(return_type, rest) \
    __declspec(deprecated) SAMPGDK_API(return_type, rest)
#else
  #define SAMPGDK_DEPRECATED_API(return_type, rest)
#endif

#if SAMPGDK_WINDOWS
  #ifdef _MSC_VER
    #define SAMPGDK_CDECL __cdecl
    #define SAMPGDK_STDCALL __stdcall
  #else
    #ifdef SAMPGDK_64BIT
      #define SAMPGDK_CDECL
      #define SAMPGDK_STDCALL
    #else
      #define SAMPGDK_CDECL __attribute__((cdecl))
      #define SAMPGDK_STDCALL __attribute__((stdcall))
    #endif
  #endif
#elif SAMPGDK_LINUX
  #ifdef SAMPGDK_64BIT
    #define SAMPGDK_CDECL
    #define SAMPGDK_STDCALL
  #else
    #define SAMPGDK_CDECL __attribute__((cdecl))
    #define SAMPGDK_STDCALL __attribute__((stdcall))
  #endif
#endif

#if SAMPGDK_LINUX && defined IN_SAMPGDK && !defined _GNU_SOURCE
  #define _GNU_SOURCE
#endif

#endif /* !SAMPGDK_PLATFORM_H */

#ifndef SAMPGDK_SDK_H
#define SAMPGDK_SDK_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/platform.h> */

/* stdint.h */
#if !defined HAVE_STDINT_H
  #if (!defined __STDC__ && __STDC_VERSION__ >= 199901L /* C99 or newer */)\
    || (defined _MSC_VER && _MSC_VER >= 1600 /* Visual Studio 2010 and later */)\
    || defined __GNUC__ /* GCC, MinGW, etc */
    #define HAVE_STDINT_H 1
  #endif
#endif

/* size_t */
#include <stddef.h>

/* alloca() */
#if SAMPGDK_WINDOWS
  #undef HAVE_ALLOCA_H
  #include <malloc.h> /* for _alloca() */
  #if !defined alloca
    #define alloca _alloca
  #endif
#elif SAMPGDK_LINUX
  #if defined __GNUC__
    #define HAVE_ALLOCA_H 1
    #if !defined alloca
      #define alloca __builtin_alloca
    #endif
  #endif
#endif

#if defined __INTEL_COMPILER
  /* ... */
#elif defined __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wignored-attributes"
#elif defined __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wattributes"
#endif

#include "amx/amx.h"
#include "plugincommon.h"

#if defined __INTEL_COMPILER
  /* ... */
#elif defined __clang_
  #pragma clang diagnostic pop
#elif defined __GNUC__
  #pragma GCC diagnostic pop
#endif

/**
 * \addtogroup sdk
 * @{
 */

/**
 * \brief Gets called before Load() to check for compatibility
 *
 * The Supports() function indicates what possibilities this
 * plugin has. The SUPPORTS_VERSION flag is required to check
 * for compatibility with the server.
 *
 * \returns combination of SUPPORTS_* flags
 */
PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports();

/**
 * \brief Gets called when the plugin is loaded
 *
 * The Load() function gets passed on exported functions from
 * the SA-MP Server, like the AMX Functions and logprintf().
 * Should return true if loading the plugin has succeeded.
 *
 * \param ppData plugin data
 *
 * \returns \c true if the plugin has successfully loaded and
 * \c false otherwise
 */
PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData);

/**
 * \brief Gets called when the plugin is unloaded
 *
 * The Unload() function is called when the server shuts down,
 * meaning this plugin gets shut down with it.
 */
PLUGIN_EXPORT void PLUGIN_CALL Unload();

/**
 * \brief Gets called when a new script is loaded
 *
 * The AmxLoad() function gets called when a new gamemode or
 * filterscript gets loaded with the server. In here we register
 * the native functions we like to add to the scripts.
 *
 * \param amx pointer to the script's AMX object
 *
 * \returns one of AMX error codes
 */
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx);

/**
 * \brief Gets called when a script is unloaded
 *
 * When a gamemode is over or a filterscript gets unloaded, this
 * function gets called. No special actions needed in here.
 *
 * \param amx pointer to the script's AMX object
 *
 * \returns one of AMX error codes
 */
PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx);

/**
 * \brief Gets called on every server tick
 *
 * Each tick corresponds to one iteration of the server's internal
 * event loop. The interval between ticks depends on many factors,
 * but it's possible to set the minimum tick rate via server.cfg
 * (default is 5ms).
 */
PLUGIN_EXPORT void PLUGIN_CALL ProcessTick();

/** @} */

#endif /* !SAMPGDK_SDK_H */

#ifndef SAMPGDK_EXPORT_H
#define SAMPGDK_EXPORT_H

/* #include <sampgdk/platform.h> */
/* #include <sampgdk/sdk.h> */

#undef SAMPGDK_EXPORT
#undef SAMPGDK_CALL

#ifdef __cplusplus
  #define SAMPGDK_EXTERN_C extern "C"
#else
  #define SAMPGDK_EXTERN_C
#endif

#if defined SAMPGDK_STATIC || defined SAMPGDK_AMALGAMATION
  #define SAMPGDK_EMBEDDED
#endif

#ifdef SAMPGDK_EMBEDDED
  #define SAMPGDK_CALL
#else
  #define SAMPGDK_CALL SAMPGDK_CDECL
#endif

#ifdef SAMPGDK_EMBEDDED
  #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C
#else
  #if SAMPGDK_LINUX
    #if defined IN_SAMPGDK
      #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C __attribute__((visibility("default")))
    #else
      #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C
    #endif
  #elif SAMPGDK_WINDOWS
    #if defined IN_SAMPGDK
      #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C __declspec(dllexport)
    #else
      #define SAMPGDK_EXPORT SAMPGDK_EXTERN_C __declspec(dllimport)
    #endif
  #else
    #error Unsupported operating system
  #endif
#endif

#define SAMPGDK_API(return_type, rest) \
  SAMPGDK_EXPORT return_type SAMPGDK_CALL rest

#undef SAMPGDK_NATIVE_EXPORT
#undef SAMPGDK_NATIVE_CALL

#define SAMPGDK_NATIVE_EXPORT SAMPGDK_EXPORT
#define SAMPGDK_NATIVE_CALL SAMPGDK_CALL
#define SAMPGDK_NATIVE(return_type, rest) \
  SAMPGDK_NATIVE_EXPORT return_type SAMPGDK_NATIVE_CALL sampgdk_ ## rest

#undef SAMPGDK_CALLBACK_EXPORT
#undef SAMPGDK_CALLBACK_CALL

#define SAMPGDK_CALLBACK_EXPORT PLUGIN_EXPORT
#define SAMPGDK_CALLBACK_CALL PLUGIN_CALL
#define SAMPGDK_CALLBACK(return_type, rest) \
  SAMPGDK_CALLBACK_EXPORT return_type SAMPGDK_CALLBACK_CALL rest

#endif /* !SAMPGDK_EXPORT_H */

#ifndef SAMPGDK_CORE_H
#define SAMPGDK_CORE_H

#include <stdarg.h>

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/sdk.h> */

/**
 * \defgroup core      Core
 * \defgroup interop   Interop
 * \defgroup version   Version
 * \defgroup sdk       SA-MP SDK
 * \defgroup natives   SA-MP Natives
 * \defgroup callbacks SA-MP Callbacks
 */

/**
 * \addtogroup core
 * @{
 */

/**
 * \brief Hidden parameter type, do not use this
 */
typedef int sampgdk_hidden_t;

/**
 * \brief Returns supported SDK version
 *
 * This function always returns SUPPORTS_VERSION. Its sole purpose is to
 * make sure that the version of the SDK is compatible with the one that
 * was used for building the library.
 *
 * \code
 * PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
 *   return sampgdk_Supports() | SUPPORTS_PROCESS_TICK;
 * }
 * \endcode
 *
 * \returns SUPPORTS_VERSION
 */
SAMPGDK_API(unsigned int, sampgdk_Supports(void));

/**
 * \brief Initializes the library
 *
 * Allocates memory for internal data structures and sets everything
 * up. Also keeps track of currently loaded plugins and registers the
 * calling plugin for callback handling.
 *
 * This function should be called from Load().
 *
 * \param ppData pointer to plugin data as passed to Load()
 *
 * \returns \c true on success and \c false otherwise
 *
 * \see sampgdk_Unload()
 */
SAMPGDK_API(bool, sampgdk_Load(void **ppData, sampgdk_hidden_t));

/**
 * \brief Shuts everything down, opposite of sampgdk_Load()
 *
 * This function should be called from Unload().
 *
 * \see sampgdk_Load()
 */
SAMPGDK_API(void, sampgdk_Unload(sampgdk_hidden_t));

/**
 * \brief Processes timers created by the calling plugin
 *
 * Goes through the list of created timers and, if necessary, fires
 * them one by one in the order of increasing IDs.
 *
 * If timer precision is important it's better to call this function
 * on every server tick. The plugin's ProcessTick() function might be
 * a good place for that.
 */
SAMPGDK_API(void, sampgdk_ProcessTick(sampgdk_hidden_t));

/**
 * \brief Prints a message to the server log
 *
 * \note The resulting message cannot be longer than 1024 characters.
 *
 * \param format printf-style format string
 * \param ... further arguments to logprintf()
 *
 * \see sampgdk_vlogprintf()
 */
SAMPGDK_API(void, sampgdk_logprintf(const char *format, ...));

/**
 * \brief Prints a message to the server log
 *
 * This function is identica to sampgdk_logprintf() except it takes
 * a \c va_list instead of variable arguments.
 *
 * \param format printf-style format string
 * \param args further arguments to logprintf()
 *
 * \see sampgdk_logprintf()
 */
SAMPGDK_API(void, sampgdk_vlogprintf(const char *format, va_list args));

/** @} */

#define sampgdk_Load(ppData)  sampgdk_Load(ppData, 0)
#define sampgdk_Unload()      sampgdk_Unload(0)
#define sampgdk_ProcessTick() sampgdk_ProcessTick(0)

#ifdef __cplusplus

/**
 * \brief Main namespace
 */
namespace sampgdk {

/**
 * \addtogroup core
 * @{
 */

/// \brief C++ wrapper around sampgdk_Supports()
inline unsigned int Supports() {
  return sampgdk_Supports();
}

/// \brief C++ wrapper around sampgdk_Load()
inline bool Load(void **ppData) {
  return sampgdk_Load(ppData);
}

/// \brief C++ wrapper around sampgdk_Unload()
inline void Unload() {
  sampgdk_Unload();
}

/// \brief C++ wrapper around sampgdk_ProcessTick()
inline void ProcessTick() {
  sampgdk_ProcessTick();
}

/// \brief C++ wrapper around sampgdk_logprintf()
inline void logprintf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  sampgdk_vlogprintf(format, args);
  va_end(args);
}

/// \brief C++ wrapper around sampgdk_vlogprintf()
inline void vlogprintf(const char *format, va_list args) {
  sampgdk_vlogprintf(format, args);
}

/** @} */

} // namespace sampgdk

#endif /* __cplusplus */

#endif /* !SAMPGDK_CORE_H */

#ifndef SAMPGDK_VERSION_H
#define SAMPGDK_VERSION_H

/* #include <sampgdk/export.h> */

/**
 * \addtogroup version
 * @{
 */

/**
 * \brief Major version
 */
#define SAMPGDK_VERSION_MAJOR 4

/**
 * \brief Minor version
 */
#define SAMPGDK_VERSION_MINOR 6

/**
 * \brief Patch version
 */
#define SAMPGDK_VERSION_PATCH 3

/**
 * \brief Library version number in the form of \c 0xAABBCC00 where
 * \c AA, \c BB and \c CC are the major, minor and patch numbers
 */
#define SAMPGDK_VERSION_ID 67502848

/**
 * \brief Library version string in the form of \c x.y.z where \c x,
 * \c y and \c z are the major, minor and patch numbers
 */
#define SAMPGDK_VERSION_STRING "4.6.3"

/**
 * \brief Gets library version number
 *
 * \returns version number
 *
 * \see SAMPGDK_VERSION_ID
 * \see sampgdk_GetVersionString()
 */
SAMPGDK_API(int, sampgdk_GetVersion(void));

/**
 * \brief Gets library version string
 *
 * \returns version string
 *
 * \see SAMPGDK_VERSION_STRING
 * \see sampgdk_GetVersion()
 */
SAMPGDK_API(const char *, sampgdk_GetVersionString(void));

#ifdef __cplusplus

namespace sampgdk {

/// \brief C++ wrapper around sampgdk_GetVersion()
inline int GetVersion() {
  return sampgdk_GetVersion();
}

/// \brief C++ wrapper around sampgdk_GetVersionString()
inline const char *GetVersionString() {
  return sampgdk_GetVersionString();
}

} // namespace sampgdk

#endif /* __cplusplus */

/** @} */

#endif /* !SAMPGDK_VERSION_H */

#ifndef SAMPGDK_TYPES_H
#define SAMPGDK_TYPES_H

/* #include <sampgdk/export.h> */

/**
 * \brief Defines the signature of a timer callback function
 * \ingroup natives
 *
 * \param timerid timer ID as returned by SetTimer()
 * \param param user-supplied data as passed to SetTimer()
 */
typedef void (SAMPGDK_CALL *TimerCallback)(int timerid, void *param);

#endif /* !SAMPGDK_TYPES_H */

#ifndef SAMPGDK_INTEROP_H
#define SAMPGDK_INTEROP_H

#include <stdarg.h>

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */

/**
 * \addtogroup interop
 * @{
 */

/**
 * \brief Returns all currently registered native functions
 *
 * This function can be used to get the names and addresses of all native
 * functions that have been registered with amx_Register(), by both the
 * server and plugins.
 *
 * \note The returned array is NULL-terminated.
 *
 * \param number where to store the number of natives (optional).
 *
 * \returns pointer to array of registered native functions
 *
 * \see sampgdk_FindNative()
 * \see sampgdk_CallNative()
 * \see sampgdk_InvokeNative()
 */
SAMPGDK_API(const AMX_NATIVE_INFO *, sampgdk_GetNatives(int *number));

/**
 * \brief Finds a native function by name
 *
 * Searches for a native function with the specified name and returns its
 * address. In order to be found the function must be registered with
 * amx_Register() prior to the call.
 *
 * \param name name of the native function
 *
 * \returns function's address or \c NULL if not found
 *
 * \see sampgdk_GetNatives()
 * \see sampgdk_CallNative()
 * \see sampgdk_InvokeNative()
 */
SAMPGDK_API(AMX_NATIVE, sampgdk_FindNative(const char *name));

/**
 * \brief Calls a native function
 *
 * This function is suitable for calling simple natives that either have only
 * value parameters or don't have any parameters at all. If you have to pass
 * a reference or a string use sampgdk_InvokeNative() instead.
 *
 * \note The first element of \p params must contain the number of arguments
 * multiplied by \c sizeof(cell).
 *
 * \param native pointer to the native function
 * \param params parameters to be passed to the function as its second argument
 *
 * \returns function's return value
 *
 * \see sampgdk_GetNatives()
 * \see sampgdk_FindNative()
 * \see sampgdk_InvokeNative()
 */
SAMPGDK_API(cell, sampgdk_CallNative(AMX_NATIVE native, cell *params));

/**
 * \brief Calls a native function with arguments
 *
 * Argument types are specified via \p format where each character, or
 * *specifier*, corresponds to a single argument. The following format
 * specifiers are supported:
 *
 * Specifier | C/C++ type    | Description
 * :-------- | :------------ | :------------------------------------------
 * i         | int           | integer value
 * d         | int           | integer value (same as 'i')
 * b         | bool          | boolean value
 * f         | double        | floating-point value
 * r         | const cell *  | const reference (input only)
 * R         | cell *        | non-const reference (both input and output)
 * s         | const char *  | const string (input only)
 * S         | char *        | non-const string (both input and output)
 * a         | const cell *  | const array (input only)
 * A         | cell *        | non-const array (both input and output)
 *
 * \remarks For the 'S', 'a' and 'A' specifiers you have to specify the size
 * of the string/array in square brackets, e.g. "a[100]" (fixed size)
 * or s[*2] (size passed via 2nd argument).
 *
 * \note In Pawn variadic functions always take their variable arguments
 * (those represented by "...") by reference. This means that for such
 * functions you have to use the 'r' specifier where you would normally
 * use 'b', 'i' 'd' or 'f'.
 *
 * \param native pointer to the native function.
 * \param format argument types
 * \param ... arguments themselves
 *
 * \returns function's return value
 *
 * \see sampgdk_GetNatives()
 * \see sampgdk_FindNative()
 * \see sampgdk_InvokeNativeV()
 * \see sampgdk_InvokeNativeArray()
 */
SAMPGDK_API(cell, sampgdk_InvokeNative(AMX_NATIVE native,
    const char *format, ...));

/**
* \brief Calls a native function with arguments
*
* This function is identical to sampgdk_InvokeNative() except it takes
* \c va_list instead of variable arguments.
*
* \see sampgdk_GetNatives()
* \see sampgdk_FindNative()
* \see sampgdk_InvokeNative()
* \see sampgdk_InvokeNativeArray()
*/
SAMPGDK_API(cell, sampgdk_InvokeNativeV(AMX_NATIVE native,
    const char *format, va_list args));

/**
* \brief Calls a native function with an array of arguments
*
* This function is similar to sampgdk_InvokeNative() but the arguments
* are passed as an array where each element is a pointer pointing to
* the actual value.
*
* Argument types are specified via \p format where each character, or
* *specifier*, corresponds to a single argument. See sampgdk_InvokeNative()
* for the list of supported format specifiers.
*
* \param native pointer to the native function.
* \param format argument types
* \param args arguments themselves
*
* \returns function's return value
*
* \see sampgdk_GetNatives()
* \see sampgdk_FindNative()
* \see sampgdk_InvokeNative()
*/
SAMPGDK_API(cell, sampgdk_InvokeNativeArray(AMX_NATIVE native,
    const char *format, void **args));

/**
 * \brief A generic catch-all callback that gets called whenever some
 * AMX public function is executed
 *
 * This is the publics "filter" callback. It is called whenever the
 * server calls \c amx_Exec(), which practically means that you can
 * use it to hook *any* callback, even those that are called by other
 * plugins.
 *
 * \param amx AMX on which the function is called
 * \param name function name
 * \param params function arguments as stored on the AMX stack, with
 *        \c params[0] being set to the number of arguments multiplied
 *        by \c sizeof(cell)
 * \param retval where to store the return value (can be \c NULL)
 *
 * \returns \c true if the public is allowed to execute
 */
SAMPGDK_CALLBACK(bool, OnPublicCall(AMX *amx, const char *name,
    cell *params, cell *retval));

/**
 * \brief A generic catch-all callback that gets called whenever some
 * AMX public function is executed
 *
 * This callback is similar to \c OnPublicCall but also allows you to
 * stop the call from being propagated to other plugins or the gamemode
 * by setting the \c stop parameter to \c true.
 *
 * \param amx AMX on which the function is called
 * \param name function name
 * \param params function arguments as stored on the AMX stack, with
 *        \c params[0] being set to the number of arguments multiplied
 *        by \c sizeof(cell)
 * \param retval where to store the return value (can be \c NULL)
 * \param stop whether to stop public call propagation (\c false by default)
 *
 * \returns \c true if the public is allowed to execute
 */
SAMPGDK_CALLBACK(bool, OnPublicCall2(AMX *amx, const char *name,
    cell *params, cell *retval, bool *stop));

/** @} */

#ifdef __cplusplus

namespace sampgdk {

/**
  * \addtogroup interop
  * @{
  */

/// \brief C++ wrapper around sampgdk_GetNatives()
inline const AMX_NATIVE_INFO *GetNatives(int &number) {
  return sampgdk_GetNatives(&number);
}

/// \brief C++ wrapper around sampgdk_GetNatives()
inline const AMX_NATIVE_INFO *GetNatives() {
  return sampgdk_GetNatives(0);
}

/// \brief C++ wrapper around sampgdk_FindNative()
inline AMX_NATIVE FindNative(const char *name) {
  return sampgdk_FindNative(name);
}

/// \brief C++ wrapper around sampgdk_CallNative()
inline cell CallNative(AMX_NATIVE native, cell *params) {
  return sampgdk_CallNative(native, params);
}

/// \brief C++ wrapper around sampgdk_InvokeNative()
inline cell InvokeNative(AMX_NATIVE native, const char *format, ...) {
  va_list args;
  va_start(args, format);
  cell retval = sampgdk_InvokeNativeV(native, format, args);
  va_end(args);
  return retval;
}

/// \brief C++ wrapper around sampgdk_InvokeNativeV()
inline cell InvokeNativeV(AMX_NATIVE native, const char *format,
    va_list args) {
  return sampgdk_InvokeNativeV(native, format, args);
}

/// \brief C++ wrapper around sampgdk_InvokeNativeArray()
inline cell InvokeNativeArray(AMX_NATIVE native, const char *format,
    void **args) {
  return sampgdk_InvokeNativeArray(native, format, args);
}

/** @} */

} // namespace sampgdk

#endif /* __cplusplus */

#endif /* !SAMPGDK_INTEROP_H */

#ifndef SAMPGDK_A_HTTP_H
#define SAMPGDK_A_HTTP_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

SAMPGDK_CALLBACK(void, OnHTTPResponse(int index, int response_code, const char * data));

#endif /* !SAMPGDK_A_HTTP_H */

#ifndef SAMPGDK_A_SAMP_H
#define SAMPGDK_A_SAMP_H

/* #include <sampgdk/bool.h> */
/* #include <sampgdk/export.h> */
/* #include <sampgdk/types.h> */


#ifndef DOXYGEN

#if defined SAMPGDK_CPP_WRAPPERS && !defined IN_SAMPGDK

namespace sampgdk {

} // namespace sampgdk

#else /* SAMPGDK_CPP_WRAPPERS && !IN_SAMPGDK */

#endif /* !SAMPGDK_CPP_WRAPPERS || IN_SAMPGDK */
#endif /* !DOXYGEN */

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnGameModeInit">OnGameModeInit on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnGameModeInit());

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnGameModeExit">OnGameModeExit on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnGameModeExit());

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerConnect">OnPlayerConnect on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerConnect(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerDisconnect">OnPlayerDisconnect on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerDisconnect(int playerid, int reason));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerSpawn">OnPlayerSpawn on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerSpawn(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerDeath">OnPlayerDeath on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerDeath(int playerid, int killerid, int reason));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnVehicleSpawn">OnVehicleSpawn on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleSpawn(int vehicleid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnVehicleDeath">OnVehicleDeath on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleDeath(int vehicleid, int killerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerText">OnPlayerText on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerText(int playerid, const char * text));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerCommandText">OnPlayerCommandText on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerCommandText(int playerid, const char * cmdtext));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerRequestClass">OnPlayerRequestClass on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerRequestClass(int playerid, int classid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerEnterVehicle">OnPlayerEnterVehicle on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEnterVehicle(int playerid, int vehicleid, bool ispassenger));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerExitVehicle">OnPlayerExitVehicle on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerExitVehicle(int playerid, int vehicleid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerStateChange">OnPlayerStateChange on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerStateChange(int playerid, int newstate, int oldstate));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerEnterCheckpoint">OnPlayerEnterCheckpoint on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEnterCheckpoint(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerLeaveCheckpoint">OnPlayerLeaveCheckpoint on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerLeaveCheckpoint(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerEnterRaceCheckpoint">OnPlayerEnterRaceCheckpoint on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEnterRaceCheckpoint(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerLeaveRaceCheckpoint">OnPlayerLeaveRaceCheckpoint on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerLeaveRaceCheckpoint(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnRconCommand">OnRconCommand on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnRconCommand(const char * cmd));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerRequestSpawn">OnPlayerRequestSpawn on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerRequestSpawn(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnObjectMoved">OnObjectMoved on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnObjectMoved(int objectid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerObjectMoved">OnPlayerObjectMoved on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerObjectMoved(int playerid, int objectid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerPickUpPickup">OnPlayerPickUpPickup on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerPickUpPickup(int playerid, int pickupid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnVehicleMod">OnVehicleMod on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleMod(int playerid, int vehicleid, int componentid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnEnterExitModShop">OnEnterExitModShop on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnEnterExitModShop(int playerid, bool enterexit, int interiorid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnVehiclePaintjob">OnVehiclePaintjob on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnVehiclePaintjob(int playerid, int vehicleid, int paintjobid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnVehicleRespray">OnVehicleRespray on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleRespray(int playerid, int vehicleid, int color1, int color2));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnVehicleDamageStatusUpdate">OnVehicleDamageStatusUpdate on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleDamageStatusUpdate(int vehicleid, int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnUnoccupiedVehicleUpdate">OnUnoccupiedVehicleUpdate on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnUnoccupiedVehicleUpdate(int vehicleid, int playerid, int passenger_seat, float new_x, float new_y, float new_z, float vel_x, float vel_y, float vel_z));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerSelectedMenuRow">OnPlayerSelectedMenuRow on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerSelectedMenuRow(int playerid, int row));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerExitedMenu">OnPlayerExitedMenu on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerExitedMenu(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerInteriorChange">OnPlayerInteriorChange on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerInteriorChange(int playerid, int newinteriorid, int oldinteriorid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerKeyStateChange">OnPlayerKeyStateChange on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerKeyStateChange(int playerid, int newkeys, int oldkeys));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnRconLoginAttempt">OnRconLoginAttempt on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnRconLoginAttempt(const char * ip, const char * password, bool success));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerUpdate">OnPlayerUpdate on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerUpdate(int playerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerStreamIn">OnPlayerStreamIn on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerStreamIn(int playerid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerStreamOut">OnPlayerStreamOut on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerStreamOut(int playerid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnVehicleStreamIn">OnVehicleStreamIn on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleStreamIn(int vehicleid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnVehicleStreamOut">OnVehicleStreamOut on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleStreamOut(int vehicleid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnActorStreamIn">OnActorStreamIn on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnActorStreamIn(int actorid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnActorStreamOut">OnActorStreamOut on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnActorStreamOut(int actorid, int forplayerid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnDialogResponse">OnDialogResponse on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnDialogResponse(int playerid, int dialogid, int response, int listitem, const char * inputtext));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerTakeDamage">OnPlayerTakeDamage on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerTakeDamage(int playerid, int issuerid, float amount, int weaponid, int bodypart));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerGiveDamage">OnPlayerGiveDamage on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerGiveDamage(int playerid, int damagedid, float amount, int weaponid, int bodypart));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerGiveDamageActor">OnPlayerGiveDamageActor on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerGiveDamageActor(int playerid, int damaged_actorid, float amount, int weaponid, int bodypart));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerClickMap">OnPlayerClickMap on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickMap(int playerid, float fX, float fY, float fZ));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerClickTextDraw">OnPlayerClickTextDraw on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickTextDraw(int playerid, int clickedid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerClickPlayerTextDraw">OnPlayerClickPlayerTextDraw on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickPlayerTextDraw(int playerid, int playertextid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnIncomingConnection">OnIncomingConnection on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnIncomingConnection(int playerid, const char * ip_address, int port));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnTrailerUpdate">OnTrailerUpdate on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnTrailerUpdate(int playerid, int vehicleid));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnVehicleSirenStateChange">OnVehicleSirenStateChange on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnVehicleSirenStateChange(int playerid, int vehicleid, int newstate));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerClickPlayer">OnPlayerClickPlayer on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerClickPlayer(int playerid, int clickedplayerid, int source));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerEditObject">OnPlayerEditObject on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEditObject(int playerid, bool playerobject, int objectid, int response, float fX, float fY, float fZ, float fRotX, float fRotY, float fRotZ));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerEditAttachedObject">OnPlayerEditAttachedObject on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerEditAttachedObject(int playerid, int response, int index, int modelid, int boneid, float fOffsetX, float fOffsetY, float fOffsetZ, float fRotX, float fRotY, float fRotZ, float fScaleX, float fScaleY, float fScaleZ));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerSelectObject">OnPlayerSelectObject on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerSelectObject(int playerid, int type, int objectid, int modelid, float fX, float fY, float fZ));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerWeaponShot">OnPlayerWeaponShot on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerWeaponShot(int playerid, int weaponid, int hittype, int hitid, float fX, float fY, float fZ));

/**
 * \ingroup callbacks
 * \see <a href="http://wiki.sa-mp.com/wiki/OnPlayerRequestDownload">OnPlayerRequestDownload on SA-MP Wiki</a>
 */
SAMPGDK_CALLBACK(bool, OnPlayerRequestDownload(int playerid, int type, int crc));

#endif /* !SAMPGDK_A_SAMP_H */

