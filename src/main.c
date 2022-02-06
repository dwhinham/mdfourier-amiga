/**
 * mdfourier-amiga
 * Copyright (C) 2022 Dale Whinham <daleyo@gmail.com>
 *
 * main.c
 * Program entry point.
 */

#include <clib/alib_protos.h>
#include <hardware/cia.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define OPTPARSE_IMPLEMENTATION
#include <optparse.h>

#include "utility.h"
#include "vblank.h"
#include "waveform.h"

extern struct CIA ciaa;
extern struct Custom custom;

static uint8_t signal_bit = 0;
static struct Task* task = NULL;

static uint32_t frame_counter = 0;

/* static int8_t __chip noise_samples[65535]; */

/* ~8000Hz */
#define SYNC_PULSE_PERIOD_PAL	222
#define SYNC_PULSE_PERIOD_NTSC	222

#define SYNC_PULSE_LEN_FRAMES	20
#define SILENCE_LEN_FRAMES	50
#define SCALE_NOTE_LEN_FRAMES	10
#define SCALE_NOTES_PER_OCTAVE	12
#define SCALE_OCTAVES		7
#define SCALE_CHANNELS		4
#define SCALE_LEN_FRAMES	(SCALE_NOTES_PER_OCTAVE * SCALE_OCTAVES * SCALE_CHANNELS * SCALE_NOTE_LEN_FRAMES)

#define SAMPLE_LEN_WORDS(ARRAY) (sizeof(ARRAY) / 2)

static const int8_t* triangle_waveforms[] =
{
	waveform_triangle_octave_1,
	waveform_triangle_octave_2,
	waveform_triangle_octave_3,
	waveform_triangle_octave_4,
	waveform_triangle_octave_5,
	waveform_triangle_octave_6,
	waveform_triangle_octave_7,
};

static const size_t sample_lengths[] =
{
	SAMPLE_LEN_WORDS(waveform_triangle_octave_1),
	SAMPLE_LEN_WORDS(waveform_triangle_octave_2),
	SAMPLE_LEN_WORDS(waveform_triangle_octave_3),
	SAMPLE_LEN_WORDS(waveform_triangle_octave_4),
	SAMPLE_LEN_WORDS(waveform_triangle_octave_5),
	SAMPLE_LEN_WORDS(waveform_triangle_octave_6),
	SAMPLE_LEN_WORDS(waveform_triangle_octave_7),
};

static void sync_pulse(uint32_t frame)
{
	if (frame == 0)
	{
		/* Setup sync audio */
		custom.aud[0].ac_ptr = (UWORD*) waveform_sync_pulse;
		custom.aud[0].ac_len = sizeof(waveform_sync_pulse) / 2;
		custom.aud[0].ac_vol = 64;
		custom.aud[0].ac_per = SYNC_PULSE_PERIOD_PAL;
		custom.aud[1].ac_ptr = (UWORD*) waveform_sync_pulse;
		custom.aud[1].ac_len = sizeof(waveform_sync_pulse) / 2;
		custom.aud[1].ac_vol = 64;
		custom.aud[1].ac_per = SYNC_PULSE_PERIOD_PAL;

		/* Start sync audio */
		custom.dmacon = (DMAF_SETCLR | DMAF_AUD0 | DMAF_AUD1);
	}
	else
	{
		/* Toggle volume during sync pulses */
		uint8_t vol = frame_counter & 1 ? 0 : 64;
		custom.aud[0].ac_vol = vol;
		custom.aud[1].ac_vol = vol;
	}
}

static void silence(uint32_t frame)
{
	/* Stop audio on first frame */
	if (frame == 0)
	{
		custom.aud[0].ac_vol = 0;
		custom.aud[1].ac_vol = 0;
		custom.aud[2].ac_vol = 0;
		custom.aud[3].ac_vol = 0;
		custom.dmacon = DMAF_AUDIO;
	}
}

static void triangle_scale(uint32_t frame)
{
	static const uint8_t channels[] = { 0, 3, 1, 2 };
	static uint32_t note_frames = 0;
	static uint8_t current_channel_index = 0;
	static uint8_t current_note = 0;
	static uint8_t current_octave = 0;

	if (frame == 0)
	{
		/* Reset channel/note/octave */
		note_frames = 0;
		current_channel_index = 0;
		current_note = 0;
		current_octave = 0;

		/* Setup audio */
		uint8_t channel = channels[current_channel_index];
		custom.aud[channel].ac_ptr = (UWORD*) triangle_waveforms[current_octave];
		custom.aud[channel].ac_len = sample_lengths[current_octave];
		custom.aud[channel].ac_per = waveform_period_table_pal[current_note];
		custom.aud[channel].ac_vol = 64;

		/* Start audio */
		custom.dmacon = (DMAF_SETCLR | (DMAF_AUD0 << channel));

		DEBUG_LOG("channel %ld, octave %ld, note %ld\n", channel, current_octave, current_note);
	}
	else if (note_frames == SCALE_NOTE_LEN_FRAMES)
	{
		note_frames = 0;

		/* Stop audio on current channel */
		uint8_t channel = channels[current_channel_index];
		custom.aud[channel].ac_vol = 0;
		//custom.dmacon = DMAF_AUD0 << channel;

		++current_channel_index;
		if (current_channel_index == 4)
		{
			current_channel_index	= 0;
			++current_note;
			if (current_note == 12)
			{
				++current_octave;
				current_note = 0;
			}
		}

		DEBUG_LOG("channel %ld, octave %ld, note %ld\n", channel, current_octave, current_note);

		channel = channels[current_channel_index];
		custom.aud[channel].ac_ptr = (UWORD*) triangle_waveforms[current_octave];
		custom.aud[channel].ac_len = sample_lengths[current_octave];
		custom.aud[channel].ac_per = waveform_period_table_pal[current_note];
		custom.aud[channel].ac_vol = 64;
		custom.dmacon = (DMAF_SETCLR | (DMAF_AUD0 << channel));
	}

	++note_frames;
}

static void enable_led_filter(uint32_t frame)
{
	ciaa.ciapra |= CIAF_LED;
}

static void disable_led_filter(uint32_t frame)
{
	ciaa.ciapra &= ~CIAF_LED;
}

static void end(uint32_t frame)
{
	/* Stop audio */
	custom.aud[0].ac_vol = 0;
	custom.aud[1].ac_vol = 0;
	custom.aud[2].ac_vol = 0;
	custom.aud[3].ac_vol = 0;
	custom.dmacon = DMAF_AUDIO;
	DEBUG_LOG("Disabling vblank\n");
	vblank_enable_server(false);
}

typedef void (*test_func_t)(uint32_t frame);

typedef struct
{
	uint32_t length_frames;
	test_func_t function;
	const char* name;
} test_phase_t;

static const test_phase_t timeline[] =
{
	{ 0,				disable_led_filter,		NULL							},
	{ SYNC_PULSE_LEN_FRAMES,	sync_pulse,			"Start sync pulse"					},
	{ SILENCE_LEN_FRAMES,		silence,			"Silence"						},
	{ SCALE_LEN_FRAMES,		triangle_scale,			"Triangle wave scale; channels 0, 3, 1, 2 (LED off)"	},
	{ 0,				enable_led_filter,		NULL							},
	{ SCALE_LEN_FRAMES,		triangle_scale,			"Triangle wave scale; channels 0, 3, 1, 2 (LED on)"	},
	{ 0,				disable_led_filter,		NULL							},
	{ SILENCE_LEN_FRAMES,		silence,			"Silence"						},
	{ SYNC_PULSE_LEN_FRAMES,	sync_pulse,			"End sync pulse"					},
	{ 0,				end,				NULL							},
	{ 0,				NULL,				NULL							},
};

static const test_phase_t* current_phase = &timeline[0];
static uint32_t current_phase_frames = 0;

static void vblank_proc()
{
	/* End of non-zero-frame phase */
	if (current_phase->length_frames != 0 && current_phase_frames == current_phase->length_frames)
	{
		current_phase_frames = 0;
		++current_phase;
		Signal(task, 1 << signal_bit);
	}

	/* Immediately execute zero-frame phases and move to the next */
	while (current_phase->length_frames == 0 && current_phase->function != NULL)
	{
		current_phase->function(0);
		++current_phase;
		Signal(task, 1 << signal_bit);
	}

	/* Execute non-zero-frame phase and update frame counter */
	if (current_phase->function)
	{
		current_phase->function(current_phase_frames);
		++current_phase_frames;
		++frame_counter;
	}
}

static void print_usage()
{
	printf
	(
		"mdfourier-amiga \xA9 2022 Dale Whinham, Artemio Urbina\n"
		"Paula tone generator for the MDFourier audio signature analyzer.\n\n"
		"-h/--help:    Show this help text\n"
		"-v/--verbose: Enable verbose logging\n"
	);
}

int main(int argc, char **argv)
{
	static const struct optparse_long longopts[] =
	{
		{ "help",	'h',	OPTPARSE_NONE		},
		{ "verbose",	'v',	OPTPARSE_OPTIONAL	},
		{ 0						}
	};

	bool verbose = false;

	struct optparse options;
	int option;

	optparse_init(&options, argv);
	while ((option = optparse_long(&options, longopts, NULL)) != -1)
	{
		switch (option)
		{
		case 'v':
			verbose = true;
			break;

		case 'h':
			print_usage(argv[0]);
			return EXIT_SUCCESS;
			break;

		case '?':
			fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
			return EXIT_FAILURE;
		}
	}

	/* Allocate signal bit */
	BYTE signal_number = AllocSignal(-1);
	if (signal_number == -1)
	{
		fprintf(stderr, "Failed to allocate signal\n");
		return EXIT_FAILURE;
	}

	/* Find our task */
	signal_bit = signal_number;
	task = FindTask(NULL);

	/*
	printf("Generating noise samples...");
	for (size_t i = 0; i < sizeof(noise_samples); ++i)
		noise_samples[i] = (int8_t) RangeRand(UINT8_MAX);
	printf("done!\n");
	*/

	vblank_install_server(vblank_proc);
	vblank_enable_server(true);

	/* Wait to be signalled by vblank */
	while (current_phase->function != NULL)
	{
		if (verbose && current_phase->name != NULL)
			printf("%s (%d frames)\n", current_phase->name, current_phase->length_frames);

		ULONG signal = Wait(1 << signal_bit | SIGBREAKF_CTRL_C);

		/* CTRL-C was pressed, kill audio and bail out */
		if (signal & SIGBREAKF_CTRL_C)
		{
			vblank_enable_server(false);
			end(0);
			break;
		}
	}

	if (verbose)
		printf("Total frames: %d\n", frame_counter);

	vblank_remove_server();
	FreeSignal(signal_bit);

	return EXIT_SUCCESS;
}
