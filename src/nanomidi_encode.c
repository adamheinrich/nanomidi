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

/** @defgroup encoder MIDI Encoder
 *
 * @brief MIDI message encoder
 */

#include <nanomidi/nanomidi.h>
#include <assert.h>
#include <stdbool.h>
#include "nanomidi_internal.h"

/**@{*/

static bool prepare_write(struct midi_ostream *stream, size_t length)
{
	if (stream->capacity == MIDI_STREAM_CAPACITY_UNLIMITED) {
		return true;
	} else if (stream->capacity >= length) {
		stream->capacity -= length;
		return true;
	}

	return false;
}

/** @brief Encode a single MIDI message
 *
 * @param stream Pointer to the @ref midi_ostream structure
 * @param[in] msg Pointer to the @ref midi_message structure to be encoded
 *
 * @return `true` if the message has been encoded, `false` otherwise
 * (either the message type is unknown or the output stream fails to write
 * encoded data).
 */
size_t midi_encode(struct midi_ostream *stream, const struct midi_message *msg)
{
	assert(stream != NULL);
	assert(msg != NULL);
	assert(stream->write_cb != NULL);

	char buffer[3];
	size_t length;

	switch (msg->type) {
	case MIDI_TYPE_NOTE_ON:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.note_on.note);
		buffer[2] = DATA_BYTE(msg->data.note_on.velocity);
		break;
	case MIDI_TYPE_NOTE_OFF:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.note_off.note);
		buffer[2] = DATA_BYTE(msg->data.note_off.velocity);
		break;
	case MIDI_TYPE_POLYPHONIC_PRESSURE:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.polyphonic_pressure.note);
		buffer[2] = DATA_BYTE(msg->data.polyphonic_pressure.pressure);
		break;
	case MIDI_TYPE_CONTROL_CHANGE:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.control_change.controller);
		buffer[2] = DATA_BYTE(msg->data.control_change.value);
		break;
	case MIDI_TYPE_PROGRAM_CHANGE:
		length = 2;
		buffer[1] = DATA_BYTE(msg->data.program_change.program);
		break;
	case MIDI_TYPE_CHANNEL_PRESSURE:
		length = 2;
		buffer[1] = DATA_BYTE(msg->data.channel_pressure.pressure);
		break;
	case MIDI_TYPE_PITCH_BEND:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.pitch_bend.value);
		buffer[2] = DATA_BYTE(msg->data.pitch_bend.value >> 7);
		break;
	case MIDI_TYPE_TIME_CODE_QUARTER_FRAME:
		length = 2;
		buffer[1] = DATA_BYTE(msg->data.time_code_quarter_frame.value);
		break;
	case MIDI_TYPE_SONG_POSITION:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.song_position.position);
		buffer[2] = DATA_BYTE(msg->data.song_position.position >> 7);
		break;
	case MIDI_TYPE_SONG_SELECT:
		length = 2;
		buffer[1] = DATA_BYTE(msg->data.song_select.song);
		break;
	case MIDI_TYPE_TUNE_REQUEST:
	case MIDI_TYPE_TIMING_CLOCK:
	case MIDI_TYPE_START:
	case MIDI_TYPE_CONTINUE:
	case MIDI_TYPE_STOP:
	case MIDI_TYPE_ACTIVE_SENSE:
	case MIDI_TYPE_SYSTEM_RESET:
		length = 1;
		break;
	default:
		length = 0;
		break;
	}

	if (length > 0) {
		if (msg->type >= (unsigned int)MIDI_TYPE_SYSTEM_BASE) {
			buffer[0] = msg->type;
		} else {
			buffer[0] = (char)(msg->type & 0xf0);
			buffer[0] = (char)(buffer[0] | (msg->channel & 0x0f));
		}

		if (!prepare_write(stream, length))
			return false;

		return stream->write_cb(stream, buffer, length);
	} else if (msg->type == MIDI_TYPE_SYSEX) {
		buffer[0] = (char)MIDI_TYPE_SOX;
		buffer[1] = (char)MIDI_TYPE_EOX;

		if (msg->data.sysex.data == NULL)
			length = 2;
		else
			length = msg->data.sysex.length + 2;

		if (!prepare_write(stream, length))
			return false;

		size_t n = stream->write_cb(stream, buffer, 1);
		if (msg->data.sysex.data != NULL) {
			for (size_t i = 0; i < msg->data.sysex.length; i++) {
				char c = DATA_BYTE(msg->data.sysex.data[i]);
				n += stream->write_cb(stream, &c, 1);
			}
		}
		n += stream->write_cb(stream, &buffer[1], 1);

		return n;
	}

	return false;
}

/**@}*/
