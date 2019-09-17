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

#ifdef ARDUINO
#include <../include/nanomidi/decoder.h>
#else
#include <nanomidi/decoder.h>
#endif

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "nanomidi_internal.h"

static bool read_buffer(struct midi_istream *stream, uint8_t *buffer)
{
	if (stream->capacity < 4)
		return false;

	if (stream->capacity != MIDI_STREAM_CAPACITY_UNLIMITED) {
		stream->capacity -= 4;
	}

	return (stream->read_cb(stream, buffer, 4) == 4);
}

static bool decode_sysex(struct midi_istream *stream, const uint8_t *buffer,
			 size_t length)
{
	if (stream->sysex_buffer.data == NULL || stream->bytes_left < 0)
		return false;

	int len;
	uint8_t *sysex_buffer = stream->sysex_buffer.data;

	for (size_t i = 0; i < length; i++) {
		switch (buffer[i]) {
		case MIDI_TYPE_SOX:
			stream->msg.type = MIDI_TYPE_SYSEX;
			stream->msg.channel = 0;
			stream->bytes_left = 0;
			break;
		case MIDI_TYPE_EOX:
			len = stream->bytes_left;
			if (len < 0)
				len = 0;
			stream->msg.data.sysex.data = stream->sysex_buffer.data;
			stream->msg.data.sysex.length = (size_t)len;
			return true;
		default:
			len = stream->bytes_left;
			if ((size_t)len < stream->sysex_buffer.size) {
				sysex_buffer[stream->bytes_left] = buffer[i];
				stream->bytes_left++;
			}
			break;
		}
	}

	return false;
}

/**
 * Decodes a single MIDI message from USB packet.
 *
 * @ingroup decoder
 *
 * The packet format is described in
 * <a href="https://www.usb.org/sites/default/files/midi10.pdf">Universal Serial
 * Bus Device Class Definition for MIDI Devices</a>.
 *
 * If a message is decoded, it has to be processed (e.g. copied) immediately
 * as it will become invalid with the next call to midi_decode().
 *
 * @param stream                Pointer to the #midi_istream structure
 * @param[out] cable_number     Decoded cable number (0 to 15)
 *
 * @return Pointer to a decoded message (allocated in #midi_istream) or `NULL`
 * if the message has not been decoded yet.
 */
struct midi_message *midi_decode_usb(struct midi_istream *stream,
				     uint8_t *cable_number)
{
	assert(stream != NULL);
	assert(cable_number != NULL);
	assert(stream->read_cb != NULL);

	uint8_t buffer[4];
	struct midi_istream istream;
	midi_istream_from_buffer(&istream, &buffer[1], 3);

	/* Stream might already contain partially decoded message, copy it: */
	istream.msg = stream->msg;
	istream.rtmsg = stream->rtmsg;
	istream.sysex_buffer = stream->sysex_buffer;
	istream.bytes_left = stream->bytes_left;

	while (read_buffer(stream, buffer)) {
		*cable_number = (buffer[0] >> 4);
		uint8_t cin = (buffer[0] & 0x0f);

		bool sysex = false;
		size_t midi_length = 0;

		switch (cin) {
		case 0x02:
		case 0x0c:
		case 0x0d:
			midi_length = 2;
			break;
		case 0x03:
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0b:
		case 0x0e:
			midi_length = 3;
			break;
		case 0x04:
			sysex = decode_sysex(&istream, &buffer[1], 3);
			break;
		case 0x05:
			if (buffer[1] == MIDI_TYPE_EOX) {
				sysex = decode_sysex(&istream, &buffer[1], 1);
			} else {
				midi_length = 1;
			}
			break;
		case 0x06:
			sysex = decode_sysex(&istream, &buffer[1], 2);
			break;
		case 0x07:
			sysex = decode_sysex(&istream, &buffer[1], 3);
			break;
		case 0x0f:
			midi_length = 1;
			break;
		default:
			/* 0x01 and 0x02 are reserved for future use */
			break;
		}

		if (sysex) {
			stream->msg = istream.msg;
			stream->bytes_left = 0;
			return &stream->msg;
		} if (midi_length > 0) {
			/* Reset buffer pointer: */
			istream.param = &buffer[1];
			istream.capacity = midi_length;

			struct midi_message *msg = midi_decode(&istream);
			if (msg != NULL) {
				stream->msg = *msg;
				stream->bytes_left = 0;
				return &stream->msg;
			}
		}
	}

	/* Message can be unfinished, copy it: */
	stream->msg = istream.msg;
	stream->bytes_left = istream.bytes_left;

	return NULL;
}
