;
; mdfourier-amiga
; Copyright (C) 2022 Dale Whinham <daleyo@gmail.com>
;
; vblank_wrapper.s
; Assembly wrapper for C-based VBlank interrupt server procedure.
;

        xdef    _vblank_proc_wrapper
        xdef    _vblank_enabled

_vblank_proc_wrapper:
		movem.l		d2-d7/a2-a4,-(sp)
		tst.b		_vblank_enabled
		beq.b		.exit
		jsr			(a1)
.exit:
		movem.l		(sp)+,d2-d7/a2-a4
		moveq		#0,d0
		rts

_vblank_enabled:
		ds.b		1
