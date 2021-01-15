/**
 * mdfourier-amiga
 * Copyright (C) 2021 Dale Whinham <daleyo@gmail.com>
 *
 * waveform.h
 * Tone generator waveforms.
 */

#pragma once

#include <stdint.h>

extern const uint8_t waveform_period_table_pal[12];
extern const uint8_t waveform_period_table_ntsc[12];

/* Sample data must reside in chip RAM */
extern const int8_t __chip waveform_sync_pulse[2];
extern const int8_t __chip waveform_triangle_octave_1[512];
extern const int8_t __chip waveform_triangle_octave_2[256];
extern const int8_t __chip waveform_triangle_octave_3[128];
extern const int8_t __chip waveform_triangle_octave_4[64];
extern const int8_t __chip waveform_triangle_octave_5[32];
extern const int8_t __chip waveform_triangle_octave_6[16];
extern const int8_t __chip waveform_triangle_octave_7[8];
