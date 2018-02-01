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

#define LONG_STR "More than 6 bytes"

int main(void)
{
	/* Buffer large enough to fit a single decoded message */
	static char buffer[6];

	static const struct midi_message messages[] = {
		/* Messages to be encoded: */
		{ .type = MIDI_TYPE_NOTE_ON, .channel = 2,
		  .data.note_on.note = 48, .data.note_on.velocity = 10 },
		{ .type = MIDI_TYPE_NOTE_OFF, .channel = 2,
		  .data.note_off.note = 48 },
		{ .type = MIDI_TYPE_SYSEX,
		  .data.sysex.data = "\xa\xc\xa\xb", .data.sysex.length = 4 },

		/* The last message should not fit the buffer, midi_decode()
		 * will not encode anything: */
		{ .type = MIDI_TYPE_SYSEX,
		  .data.sysex.data = LONG_STR,
		  .data.sysex.length = sizeof(LONG_STR)-1 },
	};

	struct midi_ostream ostream;

	for (size_t i = 0; i < sizeof(messages)/sizeof(*messages); i++) {
		printf("Message: ");
		print_msg(&messages[i]);

		/* Make the output stream write to buffer: */
		midi_ostream_from_buffer(&ostream, buffer, sizeof(buffer));
		size_t length = midi_encode(&ostream, &messages[i]);

		printf("Encoded: ");
		print_buffer(buffer, length);
	}

	return 0;
}
