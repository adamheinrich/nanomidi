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
#include <stdint.h>
#include <nanomidi.h>
#include "common.h"

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
		{ .type = MIDI_TYPE_SYSEX,
		  .data.sysex.data = "\xa\xc\xa\xb", .data.sysex.length = 4 },
		{ .type = MIDI_TYPE_SYSEX,
		  .data.sysex.data = "\x19\x17", .data.sysex.length = 2 },
	};

	/* A buffer must be allocated to make SysEx decoding work: */
	char sysex_buffer[32];
	struct midi_istream istream = {
		.read_cb = &read_buffer,
		.sysex_buffer.data = sysex_buffer,
		.sysex_buffer.size = sizeof(sysex_buffer),
	};

	struct midi_ostream ostream = { .write_cb = &write_buffer };

	printf("Encoded messages:\n");
	for (size_t i = 0; i < sizeof(messages)/sizeof(*messages); i++) {
		print_msg(&messages[i]);
		midi_encode(&ostream, &messages[i]);
	}

	printf("\nDecoded messages:\n");
	while (true) {
		struct midi_message *message = midi_decode(&istream);
		if (message == NULL)
			break;
		print_msg(message);
	}

	return 0;
}
