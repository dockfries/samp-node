#pragma once

#include "logger.hpp"
#include "utils.hpp"

/* The sampgdk CMake target propagates SAMPGDK_STATIC via its PUBLIC compile
 * definitions; guard against a duplicate definition when that is already on
 * the command line. */
#ifndef SAMPGDK_STATIC
  #define SAMPGDK_STATIC
#endif

extern void *pAMXFunctions;
