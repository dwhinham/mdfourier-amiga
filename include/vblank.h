/**
 * mdfourier-amiga
 * Copyright (C) 2021 Dale Whinham <daleyo@gmail.com>
 *
 * vblank.h
 * VBlank interrupt server management.
 */

#pragma once

#include <stdbool.h>

typedef void (*vblank_proc_t)();

bool vblank_install_server(vblank_proc_t vblank_proc);
void vblank_remove_server();
void vblank_enable_server(bool enabled);
