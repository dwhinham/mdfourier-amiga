/**
 * mdfourier-amiga
 * Copyright (C) 2022 Dale Whinham <daleyo@gmail.com>
 *
 * waveform.c
 * Tone generator waveforms.
 */

#include "waveform.h"

const uint8_t waveform_period_table_pal[12] =
{
	252,	/* A  */
	238,	/* A# */
	224,	/* B  */
	212,	/* C  */
	200,	/* C# */
	189,	/* D  */
	178,	/* D# */
	168,	/* E  */
	159,	/* F  */
	150,	/* F# */
	141,	/* G  */
	133,	/* G# */
};

const uint8_t waveform_period_table_ntsc[12] =
{
	254,	/* A  */
	240,	/* A# */
	226,	/* B  */
	214,	/* C  */
	202,	/* C# */
	190,	/* D  */
	180,	/* D# */
	170,	/* E  */
	160,	/* F  */
	151,	/* F# */
	143,	/* G  */
	135,	/* G# */
};

// HACK: prevent zero array from going into the .bss section
asm
(
	"	.globl	_waveform_silence\n"
	"	.datachip\n"
	"_waveform_silence:\n"
	"	.byte	0\n"
	"	.byte	0\n"
);

const int8_t __chip waveform_sync_pulse[2] = { -128, 127 };

const int8_t __chip waveform_triangle_octave_1[512] =
{
	0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,
	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31,
	32,	33,	34,	35,	36,	37,	38,	39,	40,	41,	42,	43,	44,	45,	46,	47,
	48,	49,	50,	51,	52,	53,	54,	55,	56,	57,	58,	59,	60,	61,	62,	63,
	64,	65,	66,	67,	68,	69,	70,	71,	72,	73,	74,	75,	76,	77,	78,	79,
	80,	81,	82,	83,	84,	85,	86,	87,	88,	89,	90,	91,	92,	93,	94,	95,
	96,	97,	98,	99,	100,	101,	102,	103,	104,	105,	106,	107,	108,	109,	110,	111,
	112,	113,	114,	115,	116,	117,	118,	119,	120,	121,	122,	123,	124,	125,	126,	127,
	127,	127,	126,	125,	124,	123,	122,	121,	120,	119,	118,	117,	116,	115,	114,	113,
	112,	111,	110,	109,	108,	107,	106,	105,	104,	103,	102,	101,	100,	99,	98,	97,
	96,	95,	94,	93,	92,	91,	90,	89,	88,	87,	86,	85,	84,	83,	82,	81,
	80,	79,	78,	77,	76,	75,	74,	73,	72,	71,	70,	69,	68,	67,	66,	65,
	64,	63,	62,	61,	60,	59,	58,	57,	56,	55,	54,	53,	52,	51,	50,	49,
	48,	47,	46,	45,	44,	43,	42,	41,	40,	39,	38,	37,	36,	35,	34,	33,
	32,	31,	30,	29,	28,	27,	26,	25,	24,	23,	22,	21,	20,	19,	18,	17,
	16,	15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,
	0,	-1,	-2,	-3,	-4,	-5,	-6,	-7,	-8,	-9,	-10,	-11,	-12,	-13,	-14,	-15,
	-16,	-17,	-18,	-19,	-20,	-21,	-22,	-23,	-24,	-25,	-26,	-27,	-28,	-29,	-30,	-31,
	-32,	-33,	-34,	-35,	-36,	-37,	-38,	-39,	-40,	-41,	-42,	-43,	-44,	-45,	-46,	-47,
	-48,	-49,	-50,	-51,	-52,	-53,	-54,	-55,	-56,	-57,	-58,	-59,	-60,	-61,	-62,	-63,
	-64,	-65,	-66,	-67,	-68,	-69,	-70,	-71,	-72,	-73,	-74,	-75,	-76,	-77,	-78,	-79,
	-80,	-81,	-82,	-83,	-84,	-85,	-86,	-87,	-88,	-89,	-90,	-91,	-92,	-93,	-94,	-95,
	-96,	-97,	-98,	-99,	-100,	-101,	-102,	-103,	-104,	-105,	-106,	-107,	-108,	-109,	-110,	-111,
	-112,	-113,	-114,	-115,	-116,	-117,	-118,	-119,	-120,	-121,	-122,	-123,	-124,	-125,	-126,	-127,
	-128,	-127,	-126,	-125,	-124,	-123,	-122,	-121,	-120,	-119,	-118,	-117,	-116,	-115,	-114,	-113,
	-112,	-111,	-110,	-109,	-108,	-107,	-106,	-105,	-104,	-103,	-102,	-101,	-100,	-99,	-98,	-97,
	-96,	-95,	-94,	-93,	-92,	-91,	-90,	-89,	-88,	-87,	-86,	-85,	-84,	-83,	-82,	-81,
	-80,	-79,	-78,	-77,	-76,	-75,	-74,	-73,	-72,	-71,	-70,	-69,	-68,	-67,	-66,	-65,
	-64,	-63,	-62,	-61,	-60,	-59,	-58,	-57,	-56,	-55,	-54,	-53,	-52,	-51,	-50,	-49,
	-48,	-47,	-46,	-45,	-44,	-43,	-42,	-41,	-40,	-39,	-38,	-37,	-36,	-35,	-34,	-33,
	-32,	-31,	-30,	-29,	-28,	-27,	-26,	-25,	-24,	-23,	-22,	-21,	-20,	-19,	-18,	-17,
	-16,	-15,	-14,	-13,	-12,	-11,	-10,	-9,	-8,	-7,	-6,	-5,	-4,	-3,	-2,	-1,
};

const int8_t __chip waveform_triangle_octave_2[256] =
{
	0,	2,	4,	6,	8,	10,	12,	14,	16,	18,	20,	22,	24,	26,	28,	30,
	32,	34,	36,	38,	40,	42,	44,	46,	48,	50,	52,	54,	56,	58,	60,	62,
	64,	66,	68,	70,	72,	74,	76,	78,	80,	82,	84,	86,	88,	90,	92,	94,
	96,	98,	100,	102,	104,	106,	108,	110,	112,	114,	116,	118,	120,	122,	124,	126,
	127,	126,	124,	122,	120,	118,	116,	114,	112,	110,	108,	106,	104,	102,	100,	98,
	96,	94,	92,	90,	88,	86,	84,	82,	80,	78,	76,	74,	72,	70,	68,	66,
	64,	62,	60,	58,	56,	54,	52,	50,	48,	46,	44,	42,	40,	38,	36,	34,
	32,	30,	28,	26,	24,	22,	20,	18,	16,	14,	12,	10,	8,	6,	4,	2,
	0,	-2,	-4,	-6,	-8,	-10,	-12,	-14,	-16,	-18,	-20,	-22,	-24,	-26,	-28,	-30,
	-32,	-34,	-36,	-38,	-40,	-42,	-44,	-46,	-48,	-50,	-52,	-54,	-56,	-58,	-60,	-62,
	-64,	-66,	-68,	-70,	-72,	-74,	-76,	-78,	-80,	-82,	-84,	-86,	-88,	-90,	-92,	-94,
	-96,	-98,	-100,	-102,	-104,	-106,	-108,	-110,	-112,	-114,	-116,	-118,	-120,	-122,	-124,	-126,
	-128,	-126,	-124,	-122,	-120,	-118,	-116,	-114,	-112,	-110,	-108,	-106,	-104,	-102,	-100,	-98,
	-96,	-94,	-92,	-90,	-88,	-86,	-84,	-82,	-80,	-78,	-76,	-74,	-72,	-70,	-68,	-66,
	-64,	-62,	-60,	-58,	-56,	-54,	-52,	-50,	-48,	-46,	-44,	-42,	-40,	-38,	-36,	-34,
	-32,	-30,	-28,	-26,	-24,	-22,	-20,	-18,	-16,	-14,	-12,	-10,	-8,	-6,	-4,	-2,
};

const int8_t __chip waveform_triangle_octave_3[128] =
{
	0,	4,	8,	12,	16,	20,	24,	28,	32,	36,	40,	44,	48,	52,	56,	60,
	64,	68,	72,	76,	80,	84,	88,	92,	96,	100,	104,	108,	112,	116,	120,	124,
	127,	124,	120,	116,	112,	108,	104,	100,	96,	92,	88,	84,	80,	76,	72,	68,
	64,	60,	56,	52,	48,	44,	40,	36,	32,	28,	24,	20,	16,	12,	8,	4,
	0,	-4,	-8,	-12,	-16,	-20,	-24,	-28,	-32,	-36,	-40,	-44,	-48,	-52,	-56,	-60,
	-64,	-68,	-72,	-76,	-80,	-84,	-88,	-92,	-96,	-100,	-104,	-108,	-112,	-116,	-120,	-124,
	-128,	-124,	-120,	-116,	-112,	-108,	-104,	-100,	-96,	-92,	-88,	-84,	-80,	-76,	-72,	-68,
	-64,	-60,	-56,	-52,	-48,	-44,	-40,	-36,	-32,	-28,	-24,	-20,	-16,	-12,	-8,	-4,
};

const int8_t __chip waveform_triangle_octave_4[64] =
{
	0,	8,	16,	24,	32,	40,	48,	56,	64,	72,	80,	88,	96,	104,	112,	120,
	127,	120,	112,	104,	96,	88,	80,	72,	64,	56,	48,	40,	32,	24,	16,	8,
	0,	-8,	-16,	-24,	-32,	-40,	-48,	-56,	-64,	-72,	-80,	-88,	-96,	-104,	-112,	-120,
	-128,	-120,	-112,	-104,	-96,	-88,	-80,	-72,	-64,	-56,	-48,	-40,	-32,	-24,	-16,	-8,
};

const int8_t __chip waveform_triangle_octave_5[32] =
{
	0,	16,	32,	48,	64,	80,	96,	112,	127,	112,	96,	80,	64,	48,	32,	16,
	0,	-16,	-32,	-48,	-64,	-80,	-96,	-112,	-128,	-112,	-96,	-80,	-64,	-48,	-32,	-16,
};

const int8_t __chip waveform_triangle_octave_6[16] =
{
	0,	32,	64,	96,	127,	96,	64,	32,	0,	-32,	-64,	-96,	-128,	-96,	-64,	-32,
};

const int8_t __chip waveform_triangle_octave_7[8] =
{
	0,	64,	127,	64,	0,	-64,	-128,	-64,
};
