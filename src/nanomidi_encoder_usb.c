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
#include <../include/nanomidi/encoder.h>
#else
#include <nanomidi/encoder.h>
#endif

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "nanomidi_internal.h"

static size_t write_buffer(struct midi_ostream *stream, uint8_t *buffer)
{
	if (stream->capacity < 4)
		return false;
	else if (stream->capacity < MIDI_STREAM_CAPACITY_UNLIMITED)
		stream->capacity -= 4;

	return stream->write_cb(stream, buffer, 4);
}

static size_t encode_sysex(struct midi_ostream *stream,
			   const struct midi_message *msg, uint8_t cable_number)
{
	int remaining;
	const uint8_t *sdata = msg->data.sysex.data;
	size_t num_written = 0;
	uint8_t buffer[4] = { 0 };

	switch (msg->data.sysex.length) {
	case 0:
		break;
	case 1:
		/* Single-byte SysEx: */
		buffer[1] = MIDI_TYPE_SOX;
		buffer[2] = *sdata;
		buffer[3] = MIDI_TYPE_EOX;
		return write_buffer(stream, buffer);
	default:
		/* SysEx starts: */
		remaining  = (int)msg->data.sysex.length + 1;
		buffer[0] = USB_BYTE0(cable_number, 0x04);
		buffer[1] = MIDI_TYPE_SOX;
		buffer[2] = *sdata++;
		buffer[3] = *sdata++;

		while (remaining >= 0) {
			remaining -= 3;

			size_t n = write_buffer(stream, buffer);
			num_written += n;
			if (n < 4)
				return num_written;

			switch (remaining) {
			case 0:
				/* SysEx ends with single byte: */
				buffer[0] = USB_BYTE0(cable_number, 0x05);
				buffer[1] = MIDI_TYPE_EOX;
				buffer[2] = 0;
				buffer[3] = 0;
				break;
			case 1:
				/* SysEx ends with two bytes: */
				buffer[0] = USB_BYTE0(cable_number, 0x06);
				buffer[1] = *sdata++;
				buffer[2] = MIDI_TYPE_EOX;
				buffer[3] = 0;
				break;
			case 2:
				/* SysEx ends with three bytes: */
				buffer[0] = USB_BYTE0(cable_number, 0x07);
				buffer[1] = *sdata++;
				buffer[2] = *sdata++;
				buffer[3] = MIDI_TYPE_EOX;
				break;
			default:
				/* SysEx continues: */
				for (int i = 0; i < 3; i++)
					buffer[i] = *sdata++;
				break;
			}
		}

		return num_written;
	}

	return 0;
}

/**
 * Encodes a single MIDI message into USB packet.
 *
 * @ingroup encoder
 *
 * The packet format is described in
 * <a href="https://www.usb.org/sites/default/files/midi10.pdf">Universal Serial
 * Bus Device Class Definition for MIDI Devices</a>.
 *
 * @param stream        Pointer to the #midi_ostream structure
 * @param[in] msg       Pointer to the #midi_message structure to be encoded
 * @param cable_number  Cable number (0-16)
 *
 * @return The number of bytes encoded (multiples of four).
 */
size_t midi_encode_usb(struct midi_ostream *stream,
		       const struct midi_message *msg, uint8_t cable_number)
{
	assert(stream != NULL);
	assert(msg != NULL);
	assert(stream->write_cb != NULL);

	uint8_t cin = 0;

	switch (msg->type) {
	/* Single-byte System Common messages: */
	case MIDI_TYPE_TUNE_REQUEST:
		cin = 0x05;
		break;
	/* Two-byte System Common messages: */
	case MIDI_TYPE_TIME_CODE_QUARTER_FRAME:
	case MIDI_TYPE_SONG_SELECT:
		cin = 0x02;
		break;
	/* Three-byte System Common messages: */
	case MIDI_TYPE_SONG_POSITION:
		cin = 0x03;
		break;
	/* Channel Mode messages: */
	case MIDI_TYPE_NOTE_OFF:
	case MIDI_TYPE_NOTE_ON:
	case MIDI_TYPE_POLYPHONIC_PRESSURE:
	case MIDI_TYPE_CONTROL_CHANGE:
	case MIDI_TYPE_PROGRAM_CHANGE:
	case MIDI_TYPE_CHANNEL_PRESSURE:
	case MIDI_TYPE_PITCH_BEND:
		cin = (uint8_t)(msg->type >> 4);
		break;
	/* System exclusive message: */
	case MIDI_TYPE_SYSEX:
		return encode_sysex(stream, msg, cable_number);
	/* Single byte: */
	default:
		cin = 0x0f;
		break;
	}

	if (cin >= 0x02) { /* CIN 0x00 and 0x01 are reserved for future use */
		struct midi_ostream ostream;
		uint8_t buffer[4] = { USB_BYTE0(cable_number, cin) };
		midi_ostream_from_buffer(&ostream, &buffer[1], 3);

		if (midi_encode(&ostream, msg) >= 1)
			return write_buffer(stream, buffer) ? 4 : 0;
	}

	return 0;
}
