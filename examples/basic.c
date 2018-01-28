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

void print_msg(const struct midi_message *msg);

static char buffer[1024];
static size_t buffer_wr; /* Write index */
static size_t buffer_rd; /* Read index */

static int read_buffer(void *param, char *data, size_t size)
{
	(void)param;

	for (size_t i = 0; i < size; i++) {
		data[i] = buffer[buffer_rd];
		if (buffer_rd++ >= buffer_wr)
			return (int)i;
	}

	return (int)size;
}

static int write_buffer(void *param, const char *data, size_t size)
{
	(void)param;

	for (size_t i = 0; i < size; i++) {
		buffer[buffer_wr] = data[i];
		if (buffer_wr++ >= sizeof(buffer))
			return (int)i;
	}

	return (int)size;
}

int main(void)
{
	static const struct midi_message messages[] = {
		/* Channel Mode Messages: */
		{ .status = MIDI_STATUS_NOTE_ON, .channel = 2,
		  .data.note_on.note = 48, .data.note_on.velocity = 10 },
		{ .status = MIDI_STATUS_NOTE_OFF, .channel = 2,
		  .data.note_off.note = 48 },
		{ .status = MIDI_STATUS_POLYPHONIC_PRESSURE, .channel = 2,
		  .data.polyphonic_pressure.note = 68,
		  .data.polyphonic_pressure.pressure = 64 },
		{ .status = MIDI_STATUS_CONTROL_CHANGE, .channel = 2,
		  .data.control_change.controller = 5,
		  .data.control_change.value = 100 },
		{ .status = MIDI_STATUS_PROGRAM_CHANGE, .channel = 2,
		  .data.program_change.program = 42 },
		{ .status = MIDI_STATUS_CHANNEL_PRESSURE, .channel = 2,
		  .data.channel_pressure.pressure = 90 },
		{ .status = MIDI_STATUS_PITCH_BEND, .channel = 2,
		  .data.pitch_bend.value = 1234 },

		/* System Common Messages: */
		{ .status = MIDI_STATUS_SYSTEM_TIME_CODE_QUARTER_FRAME,
		  .data.system_time_code_quarter_frame.value = 101 },
		{ .status = MIDI_STATUS_SYSTEM_SONG_POSITION,
		  .data.system_song_position.position = 1917 },
		{ .status = MIDI_STATUS_SYSTEM_SONG_SELECT,
		  .data.system_song_select.song = 92 },
	};

	struct midi_istream istream = { .read_cb = &read_buffer };
	struct midi_ostream ostream = { .write_cb = &write_buffer };

	printf("Encoded messages:\n");
	for (size_t i = 0; i < sizeof(messages)/sizeof(*messages); i++) {
		print_msg(&messages[i]);
		midi_encode(&ostream, &messages[i]);
	}

	printf("\nDecoded messages:\n");
	while (true) {
		struct midi_message message;
		if (!midi_decode(&istream, &message))
			break;
		print_msg(&message);
	}

	return 0;
}
