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
	MIDI_STATUS_NOTE_ON = 0x08,
	MIDI_STATUS_NOTE_OFF,
	MIDI_STATUS_POLYPHONIC_PRESSURE,
	MIDI_STATUS_CONTROL_CHANGE,
	MIDI_STATUS_PROGRAM_CHANGE,
	MIDI_STATUS_CHANNEL_PRESSURE,
	MIDI_STATUS_PITCH_BEND,
	/* TODO: Implement MIDI_STATUS_SYSTEM */
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
	} data;
};

struct midi_istream {
	int (*read_cb)(void *param, char *data, size_t size);
	size_t bytes_left;
	void *param;
};

struct midi_ostream {
	int (*write_cb)(void *param, const char *data, size_t size);
	void *param;
};

bool midi_decode(struct midi_istream *stream, struct midi_message *msg);
bool midi_encode(struct midi_ostream *stream, const struct midi_message *msg);

#endif /* NANOMIDI_H */
