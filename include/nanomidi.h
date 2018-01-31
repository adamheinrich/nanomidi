/*
 * This file is part of nanomidi.
 *
 * Copyright (C) 2018 Adam Heinrich <adam@adamh.cz>
 *
 * Nanomidi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanomidi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with nanomidi.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NANOMIDI_H
#define NANOMIDI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum midi_type {
	/* Channel Mode Messages: */
	MIDI_TYPE_NOTE_OFF = 0x80,
	MIDI_TYPE_NOTE_ON = 0x90,
	MIDI_TYPE_POLYPHONIC_PRESSURE = 0xa0,
	MIDI_TYPE_CONTROL_CHANGE = 0xb0,
	MIDI_TYPE_PROGRAM_CHANGE = 0xc0,
	MIDI_TYPE_CHANNEL_PRESSURE = 0xd0,
	MIDI_TYPE_PITCH_BEND = 0xe0,

	/* System Common Messages: */
	MIDI_TYPE_TIME_CODE_QUARTER_FRAME = 0xf1,
	MIDI_TYPE_SONG_POSITION = 0xf2,
	MIDI_TYPE_SONG_SELECT = 0xf3,
	MIDI_TYPE_TUNE_REQUEST = 0xf6,

	/* System Real Time Messages: */
	MIDI_TYPE_TIMING_CLOCK = 0xf8,
	MIDI_TYPE_START = 0xfa,
	MIDI_TYPE_CONTINUE = 0xfb,
	MIDI_TYPE_STOP = 0xfc,
	MIDI_TYPE_ACTIVE_SENSE = 0xfe,
	MIDI_TYPE_SYSTEM_RESET = 0xff,

	/* System Exclusive Messages (SysEx): */
	MIDI_TYPE_SYSEX = 0x00,
	MIDI_TYPE_SYSTEM_EXCLUSIVE = MIDI_TYPE_SYSEX,
};

struct midi_message {
	enum midi_type type;
	uint8_t channel;
	union data {
		struct note_on {
			uint8_t note;
			uint8_t velocity;
		} note_on;
		struct note_off {
			uint8_t note;
			uint8_t velocity;
		} note_off;
		struct polyphonic_pressure {
			uint8_t note;
			uint8_t pressure;
		} polyphonic_pressure;
		struct control_change {
			uint8_t controller;
			uint8_t value;
		} control_change;
		struct program_change {
			uint8_t program;
		} program_change;
		struct channel_pressure {
			uint8_t pressure;
		} channel_pressure;
		struct pitch_bend {
			uint16_t value;
		} pitch_bend;
		struct time_code_quarter_frame {
			uint8_t value;
		} time_code_quarter_frame;
		struct song_position {
			uint16_t position;
		} song_position;
		struct song_select {
			uint8_t song;
		} song_select;
		struct sysex {
			const char *data;
			size_t length;
		} sysex;
	} data;
};

struct midi_istream {
	int (*read_cb)(void *param, char *data, size_t size);
	struct midi_message msg;
	struct midi_message rtmsg;
	struct sysex_buffer {
		char *data;
		size_t size;
	} sysex_buffer;
	int bytes_left;
	void *param;
};

struct midi_ostream {
	int (*write_cb)(void *param, const char *data, size_t size);
	void *param;
};

struct midi_message *midi_decode(struct midi_istream *stream);
bool midi_encode(struct midi_ostream *stream, const struct midi_message *msg);

#endif /* NANOMIDI_H */
