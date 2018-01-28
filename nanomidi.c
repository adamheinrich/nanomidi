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

#define DATA_BYTE(data)		((char)((data) & 0x7f))

enum midi_status_system {
	MIDI_STATUS_SYSTEM = 0xf0,
	MIDI_STATUS_SYSTEM_SOX = 0xf1,
	MIDI_STATUS_SYSTEM_EOX = 0xf7,
};

static int data_size(struct midi_message *msg)
{
	int length;

	switch (msg->status) {
	case MIDI_STATUS_NOTE_ON:
	case MIDI_STATUS_NOTE_OFF:
	case MIDI_STATUS_POLYPHONIC_PRESSURE:
	case MIDI_STATUS_CONTROL_CHANGE:
	case MIDI_STATUS_PITCH_BEND:
	case MIDI_STATUS_SYSTEM_SONG_POSITION:
		length = 2;
		break;
	case MIDI_STATUS_PROGRAM_CHANGE:
	case MIDI_STATUS_CHANNEL_PRESSURE:
	case MIDI_STATUS_SYSTEM_TIME_CODE_QUARTER_FRAME:
	case MIDI_STATUS_SYSTEM_SONG_SELECT:
		length = 1;
		break;
	case MIDI_STATUS_SYSTEM_TUNE_REQUEST:
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
	switch (msg->status) {
	case MIDI_STATUS_NOTE_ON:
		if (bytes_left == 2)
			msg->data.note_on.note = DATA_BYTE(c);
		else
			msg->data.note_on.velocity = DATA_BYTE(c);
		break;
	case MIDI_STATUS_NOTE_OFF:
		if (bytes_left == 2)
			msg->data.note_off.note = DATA_BYTE(c);
		else
			msg->data.note_off.velocity = DATA_BYTE(c);
		break;
	case MIDI_STATUS_POLYPHONIC_PRESSURE:
		if (bytes_left == 2)
			msg->data.polyphonic_pressure.note = DATA_BYTE(c);
		else
			msg->data.polyphonic_pressure.pressure = DATA_BYTE(c);
		break;
	case MIDI_STATUS_CONTROL_CHANGE:
		if (bytes_left == 2)
			msg->data.control_change.controller = DATA_BYTE(c);
		else
			msg->data.control_change.value = DATA_BYTE(c);
		break;
	case MIDI_STATUS_PROGRAM_CHANGE:
		msg->data.program_change.program = DATA_BYTE(c);
		break;
	case MIDI_STATUS_CHANNEL_PRESSURE:
		msg->data.channel_pressure.pressure = DATA_BYTE(c);
		break;
	case MIDI_STATUS_PITCH_BEND:
		if (bytes_left == 2) {
			msg->data.pitch_bend.value = DATA_BYTE(c);
		} else {
			uint16_t msb = (DATA_BYTE(c) << 7);
			msg->data.pitch_bend.value |= msb;
		}
		break;
	case MIDI_STATUS_SYSTEM_TIME_CODE_QUARTER_FRAME:
		msg->data.system_time_code_quarter_frame.value = DATA_BYTE(c);
		break;
	case MIDI_STATUS_SYSTEM_SONG_POSITION:
		if (bytes_left == 2) {
			msg->data.system_song_position.position = DATA_BYTE(c);
		} else {
			uint16_t msb = (DATA_BYTE(c) << 7);
			msg->data.system_song_position.position |= msb;
		}
		break;
	case MIDI_STATUS_SYSTEM_SONG_SELECT:
		msg->data.system_song_select.song = DATA_BYTE(c);
		break;
	default:
		return false;
	}

	return (bytes_left == 1);
}

static bool is_realtime_message(int status)
{
	bool is_rt;

	switch (status) {
	case MIDI_STATUS_SYSTEM_TIMING_CLOCK:
	case MIDI_STATUS_SYSTEM_START:
	case MIDI_STATUS_SYSTEM_CONTINUE:
	case MIDI_STATUS_SYSTEM_STOP:
	case MIDI_STATUS_SYSTEM_ACTIVE_SENSE:
	case MIDI_STATUS_SYSTEM_RESET:
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
		bool is_status_byte = ((c & 0x80) != 0);
		if (is_status_byte) {
			int status = (c & 0xff);
			if (is_realtime_message(status)) {
				stream->rtmsg.status = status;
				return &stream->rtmsg;
			} else if (status >= MIDI_STATUS_SYSTEM) {
				stream->msg.status = status;
				stream->msg.channel = 0;
			} else {
				stream->msg.status = (status & 0xf0);
				stream->msg.channel = (c & 0x0f);
			}

			stream->bytes_left = data_size(&stream->msg);
			if (stream->bytes_left == 0) /* Message with no data */
				return &stream->msg;
		} else {
			if (stream->bytes_left == 0) /* Running status */
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

bool midi_encode(struct midi_ostream *stream, const struct midi_message *msg)
{
	assert(stream != NULL);
	assert(msg != NULL);
	assert(stream->write_cb != NULL);

	char buffer[3];
	size_t length;

	switch (msg->status) {
	case MIDI_STATUS_NOTE_ON:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.note_on.note);
		buffer[2] = DATA_BYTE(msg->data.note_on.velocity);
		break;
	case MIDI_STATUS_NOTE_OFF:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.note_off.note);
		buffer[2] = DATA_BYTE(msg->data.note_off.velocity);
		break;
	case MIDI_STATUS_POLYPHONIC_PRESSURE:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.polyphonic_pressure.note);
		buffer[2] = DATA_BYTE(msg->data.polyphonic_pressure.pressure);
		break;
	case MIDI_STATUS_CONTROL_CHANGE:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.control_change.controller);
		buffer[2] = DATA_BYTE(msg->data.control_change.value);
		break;
	case MIDI_STATUS_PROGRAM_CHANGE:
		length = 2;
		buffer[1] = DATA_BYTE(msg->data.program_change.program);
		break;
	case MIDI_STATUS_CHANNEL_PRESSURE:
		length = 2;
		buffer[1] = DATA_BYTE(msg->data.channel_pressure.pressure);
		break;
	case MIDI_STATUS_PITCH_BEND:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.pitch_bend.value);
		buffer[2] = DATA_BYTE(msg->data.pitch_bend.value >> 7);
		break;
	case MIDI_STATUS_SYSTEM_TIME_CODE_QUARTER_FRAME:
		length = 2;
		buffer[1] = DATA_BYTE(msg->data.system_time_code_quarter_frame.value);
		break;
	case MIDI_STATUS_SYSTEM_SONG_POSITION:
		length = 3;
		buffer[1] = DATA_BYTE(msg->data.system_song_position.position);
		buffer[2] = DATA_BYTE(msg->data.system_song_position.position >> 7);
		break;
	case MIDI_STATUS_SYSTEM_SONG_SELECT:
		length = 2;
		buffer[1] = DATA_BYTE(msg->data.system_song_select.song);
		break;
	case MIDI_STATUS_SYSTEM_TUNE_REQUEST:
	case MIDI_STATUS_SYSTEM_TIMING_CLOCK:
	case MIDI_STATUS_SYSTEM_START:
	case MIDI_STATUS_SYSTEM_CONTINUE:
	case MIDI_STATUS_SYSTEM_STOP:
	case MIDI_STATUS_SYSTEM_ACTIVE_SENSE:
	case MIDI_STATUS_SYSTEM_RESET:
		length = 1;
		break;
	default:
		length = 0;
		break;
	}

	if (length > 0) {
		if (msg->status >= (unsigned int)MIDI_STATUS_SYSTEM) {
			buffer[0] = msg->status;
		} else {
			buffer[0] = (char)(msg->status & 0xf0);
			buffer[0] = (char)(buffer[0] | (msg->channel & 0x0f));
		}

		int n = stream->write_cb(stream->param, buffer, length);
		return (n == (int)length);
	}

	return false;
}
