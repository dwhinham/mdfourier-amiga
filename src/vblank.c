/**
 * mdfourier-amiga
 * Copyright (C) 2022 Dale Whinham <daleyo@gmail.com>
 *
 * vblank.c
 * VBlank interrupt server management.
 */

#include <exec/interrupts.h>
#include <hardware/custom.h>
#include <hardware/intbits.h>
#include <proto/exec.h>

#include "vblank.h"

extern struct Custom custom;
extern bool vblank_enabled;
extern void vblank_proc_wrapper();

static struct Interrupt* vblank_server = NULL;

bool vblank_install_server(vblank_proc_t vblank_proc)
{
	vblank_server = AllocMem(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR);
	if (!vblank_server)
		return false;

	/* Set up the VBLANK interrupt structure */
	vblank_server->is_Node.ln_Type = NT_INTERRUPT;
	vblank_server->is_Node.ln_Pri = 0;
	vblank_server->is_Node.ln_Name = "MDFourier VBlank";
	vblank_server->is_Data = vblank_proc;
	vblank_server->is_Code = vblank_proc_wrapper;

	/* Install interrupt server */
	AddIntServer(INTB_VERTB, vblank_server);

	return true;
}

void vblank_remove_server()
{
	RemIntServer(INTB_VERTB, vblank_server);
	FreeMem(vblank_server, sizeof(struct Interrupt));
}

void vblank_enable_server(bool enabled)
{
	vblank_enabled = enabled;
}
