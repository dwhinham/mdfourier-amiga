/**
 * mdfourier-amiga
 * Copyright (C) 2022 Dale Whinham <daleyo@gmail.com>
 *
 * utility.h
 * Misc. utility macros.
 */

#pragma once

#ifdef DEBUG
#include <clib/debug_protos.h>

#define DEBUG_LOG(...) kprintf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

#define _STR(X) #X
#define STR(X) _STR(X)
