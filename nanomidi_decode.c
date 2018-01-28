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

#include <nanomidi.h>
#include <assert.h>
#include "nanomidi_internal.h"

static int data_size(struct midi_message *msg)
{
	int length;

	switch (msg->type) {
	case MIDI_TYPE_NOTE_ON:
	case MIDI_TYPE_NOTE_OFF:
	case MIDI_TYPE_POLYPHONIC_PRESSURE:
	case MIDI_TYPE_CONTROL_CHANGE:
	case MIDI_TYPE_PITCH_BEND:
	case MIDI_TYPE_SONG_POSITION:
		length = 2;
		break;
	case MIDI_TYPE_PROGRAM_CHANGE:
	case MIDI_TYPE_CHANNEL_PRESSURE:
	case MIDI_TYPE_TIME_CODE_QUARTER_FRAME:
	case MIDI_TYPE_SONG_SELECT:
		length = 1;
		break;
	case MIDI_TYPE_TUNE_REQUEST:
		length = 0;
		break;
	default:
		length = -1;
		break;
	}

	return length;
}

static bool decode_data(struct midi_message *msg, char c, int bytes_left)
{
	switch (msg->type) {
	case MIDI_TYPE_NOTE_ON:
		if (bytes_left == 2)
			msg->data.note_on.note = DATA_BYTE(c);
		else
			msg->data.note_on.velocity = DATA_BYTE(c);
		break;
	case MIDI_TYPE_NOTE_OFF:
		if (bytes_left == 2)
			msg->data.note_off.note = DATA_BYTE(c);
		else
			msg->data.note_off.velocity = DATA_BYTE(c);
		break;
	case MIDI_TYPE_POLYPHONIC_PRESSURE:
		if (bytes_left == 2)
			msg->data.polyphonic_pressure.note = DATA_BYTE(c);
		else
			msg->data.polyphonic_pressure.pressure = DATA_BYTE(c);
		break;
	case MIDI_TYPE_CONTROL_CHANGE:
		if (bytes_left == 2)
			msg->data.control_change.controller = DATA_BYTE(c);
		else
			msg->data.control_change.value = DATA_BYTE(c);
		break;
	case MIDI_TYPE_PROGRAM_CHANGE:
		msg->data.program_change.program = DATA_BYTE(c);
		break;
	case MIDI_TYPE_CHANNEL_PRESSURE:
		msg->data.channel_pressure.pressure = DATA_BYTE(c);
		break;
	case MIDI_TYPE_PITCH_BEND:
		if (bytes_left == 2) {
			msg->data.pitch_bend.value = DATA_BYTE(c);
		} else {
			uint16_t msb = (DATA_BYTE(c) << 7);
			msg->data.pitch_bend.value |= msb;
		}
		break;
	case MIDI_TYPE_TIME_CODE_QUARTER_FRAME:
		msg->data.system_time_code_quarter_frame.value = DATA_BYTE(c);
		break;
	case MIDI_TYPE_SONG_POSITION:
		if (bytes_left == 2) {
			msg->data.system_song_position.position = DATA_BYTE(c);
		} else {
			uint16_t msb = (DATA_BYTE(c) << 7);
			msg->data.system_song_position.position |= msb;
		}
		break;
	case MIDI_TYPE_SONG_SELECT:
		msg->data.system_song_select.song = DATA_BYTE(c);
		break;
	default:
		return false;
	}

	return (bytes_left == 1);
}

static bool is_realtime_message(int type)
{
	bool is_rt;

	switch (type) {
	case MIDI_TYPE_TIMING_CLOCK:
	case MIDI_TYPE_START:
	case MIDI_TYPE_CONTINUE:
	case MIDI_TYPE_STOP:
	case MIDI_TYPE_ACTIVE_SENSE:
	case MIDI_TYPE_SYSTEM_RESET:
		is_rt = true;
		break;
	default:
		is_rt = false;
		break;
	}

	return is_rt;
}

struct midi_message *midi_decode(struct midi_istream *stream)
{
	assert(stream != NULL);
	assert(stream->read_cb != NULL);

	char c;
	while (stream->read_cb(stream->param, &c, 1) == 1) {
		bool is_type_byte = ((c & 0x80) != 0);
		if (is_type_byte) {
			int type = (c & 0xff);
			if (is_realtime_message(type)) {
				stream->rtmsg.type = type;
				return &stream->rtmsg;
			} else if (type >= MIDI_TYPE_SYSTEM_BASE) {
				stream->msg.type = type;
				stream->msg.channel = 0;
			} else {
				stream->msg.type = (type & 0xf0);
				stream->msg.channel = (c & 0x0f);
			}

			stream->bytes_left = data_size(&stream->msg);
			if (stream->bytes_left == 0) /* Message with no data */
				return &stream->msg;
		} else {
			if (stream->bytes_left == 0) /* Running type */
				stream->bytes_left = data_size(&stream->msg);

			if (stream->bytes_left > 0) {
				if (decode_data(&stream->msg, c,
						stream->bytes_left)) {
					stream->bytes_left = 0;
					return &stream->msg;
				} else {
					stream->bytes_left--;
				}
			}
		}
	}

	return NULL;
}