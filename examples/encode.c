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

#include <stdio.h>
#include <nanomidi.h>
#include "common.h"

#define SYSEX_SUPPORTED		1

static char buffer[1024];
static size_t buffer_wr; /* Write index */
static size_t buffer_rd; /* Read index */

static size_t read_buffer(struct midi_istream *stream, char *data, size_t size)
{
	(void)stream;

	for (size_t i = 0; i < size; i++) {
		data[i] = buffer[buffer_rd];
		if (buffer_rd++ >= buffer_wr)
			return i;
	}

	return size;
}

static size_t write_buffer(struct midi_ostream *stream, const char *data,
			   size_t size)
{
	(void)stream;

	for (size_t i = 0; i < size; i++) {
		buffer[buffer_wr] = data[i];
		if (buffer_wr++ >= sizeof(buffer))
			return i;
	}

	return size;
}

int main(void)
{
	static const struct midi_message messages[] = {
		/* Channel Mode Messages: */
		{ .type = MIDI_TYPE_NOTE_ON, .channel = 2,
		  .data.note_on.note = 48, .data.note_on.velocity = 10 },
		{ .type = MIDI_TYPE_NOTE_OFF, .channel = 2,
		  .data.note_off.note = 48 },
		{ .type = MIDI_TYPE_POLYPHONIC_PRESSURE, .channel = 2,
		  .data.polyphonic_pressure.note = 68,
		  .data.polyphonic_pressure.pressure = 64 },
		{ .type = MIDI_TYPE_CONTROL_CHANGE, .channel = 2,
		  .data.control_change.controller = 5,
		  .data.control_change.value = 100 },
		{ .type = MIDI_TYPE_PROGRAM_CHANGE, .channel = 2,
		  .data.program_change.program = 42 },
		{ .type = MIDI_TYPE_CHANNEL_PRESSURE, .channel = 2,
		  .data.channel_pressure.pressure = 90 },
		{ .type = MIDI_TYPE_PITCH_BEND, .channel = 2,
		  .data.pitch_bend.value = 1234 },

		/* System Common Messages: */
		{ .type = MIDI_TYPE_TIME_CODE_QUARTER_FRAME,
		  .data.time_code_quarter_frame.value = 101 },
		{ .type = MIDI_TYPE_SONG_POSITION,
		  .data.song_position.position = 1917 },
		{ .type = MIDI_TYPE_SONG_SELECT,
		  .data.song_select.song = 92 },
		{ .type = MIDI_TYPE_TUNE_REQUEST },

		/* System Real Time Messages: */
		{ .type = MIDI_TYPE_TIMING_CLOCK },
		{ .type = MIDI_TYPE_START },
		{ .type = MIDI_TYPE_CONTINUE },
		{ .type = MIDI_TYPE_STOP },
		{ .type = MIDI_TYPE_ACTIVE_SENSE },
		{ .type = MIDI_TYPE_SYSTEM_RESET },

		/* System Exclusive Messages (SysEx): */
		{ .type = MIDI_TYPE_SYSEX,
		  .data.sysex.data = "\xa\xc\xa\xb", .data.sysex.length = 4 },
		{ .type = MIDI_TYPE_SYSEX,
		  .data.sysex.data = "\x19\x17", .data.sysex.length = 2 },
	};

#if SYSEX_SUPPORTED
	/* A buffer must be allocated to make SysEx decoding work: */
	char sysex_buffer[32];
	struct midi_istream istream = {
		.read_cb = &read_buffer,
		.sysex_buffer.data = sysex_buffer,
		.sysex_buffer.size = sizeof(sysex_buffer),
	};
#else
	struct midi_istream istream = { .read_cb = &read_buffer };
#endif
	struct midi_ostream ostream = {
		.write_cb = &write_buffer,
		.capacity = MIDI_OSTREAM_CAPACITY_UNLIMITED,
	};

	printf("Encoded messages:\n");
	for (size_t i = 0; i < sizeof(messages)/sizeof(*messages); i++) {
		print_msg(&messages[i]);
		midi_encode(&ostream, &messages[i]);
	}

	/* All messages are now encoded in the buffer. */

	printf("\nDecoded messages:\n");
	while (1) {
		struct midi_message *message = midi_decode(&istream);
		if (message == NULL)
			break;
		print_msg(message);
	}

	return 0;
}
