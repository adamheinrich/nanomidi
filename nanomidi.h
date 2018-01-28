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

enum midi_status {
	/* Channel Mode Messages: */
	MIDI_STATUS_NOTE_ON = 0x80,
	MIDI_STATUS_NOTE_OFF = 0x90,
	MIDI_STATUS_POLYPHONIC_PRESSURE = 0xa0,
	MIDI_STATUS_CONTROL_CHANGE = 0xb0,
	MIDI_STATUS_PROGRAM_CHANGE = 0xc0,
	MIDI_STATUS_CHANNEL_PRESSURE = 0xd0,
	MIDI_STATUS_PITCH_BEND = 0xe0,

	/* System Common Messages: */
	MIDI_STATUS_SYSTEM_TIME_CODE_QUARTER_FRAME = 0xf1,
	MIDI_STATUS_SYSTEM_SONG_POSITION = 0xf2,
	MIDI_STATUS_SYSTEM_SONG_SELECT = 0xf3,
	MIDI_STATUS_SYSTEM_TUNE_REQUEST = 0xf6,
};

struct midi_message {
	enum midi_status status;
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
		struct system_time_code_quarter_frame {
			uint8_t value;
		} system_time_code_quarter_frame;
		struct system_song_position {
			uint16_t position;
		} system_song_position;
		struct system_song_select {
			uint8_t song;
		} system_song_select;
	} data;
};

struct midi_istream {
	int (*read_cb)(void *param, char *data, size_t size);
	int bytes_left;
	void *param;
};

struct midi_ostream {
	int (*write_cb)(void *param, const char *data, size_t size);
	void *param;
};

bool midi_decode(struct midi_istream *stream, struct midi_message *msg);
bool midi_encode(struct midi_ostream *stream, const struct midi_message *msg);

#endif /* NANOMIDI_H */
