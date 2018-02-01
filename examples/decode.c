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

#define SYSEX_SUPPORTED		1

static const uint8_t buffer[] = {
	0x81, 48, 64,		/* NOTE_ON: ch=1, note=48, velocity=64 */
	49, 64,			/* Running status: note=48, velocity=64 */
	50, 64,			/* Running status: note=48, velocity=64 */
	0x90, 48, 0,		/* NOTE_OFF: ch=0, note=48, velocity=0 */
	0x81, 48, 0xff, 64,	/* NOTE_ON, realtime message (RESET) injected */
	0xf1, 0x19, 0x17, 0xf7,	/* SysEx: { 0x19, 0x17 } */
	0xf1, 0xfa, 0x42, 0xf7,	/* SysEx, realtime message (START) injected */
};

static size_t read_buffer(struct midi_istream *stream, char *data, size_t size)
{
	(void)stream;
	static size_t buffer_rd;

	for (size_t i = 0; i < size; i++) {
		data[i] = (char)buffer[buffer_rd];
		if (buffer_rd++ > sizeof(buffer))
			return i;
	}

	return size;
}

int main(void)
{
#if SYSEX_SUPPORTED
	/* A buffer must be allocated to make SysEx decoding work: */
	char sysex_buffer[32];
	struct midi_istream istream = {
		.read_cb = &read_buffer,
		.capacity = MIDI_STREAM_CAPACITY_UNLIMITED,
		.sysex_buffer.data = sysex_buffer,
		.sysex_buffer.size = sizeof(sysex_buffer),
	};
#else
	struct midi_istream istream = {
		.read_cb = &read_buffer,
		.capacity = MIDI_STREAM_CAPACITY_UNLIMITED,
	};
#endif

	printf("Decoded messages:\n");

	while (1) {
		struct midi_message *message = midi_decode(&istream);
		if (message == NULL)
			break;
		print_msg(message);
	}

	return 0;
}
