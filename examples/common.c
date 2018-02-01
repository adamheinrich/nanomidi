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

#include "common.h"
#include <stdio.h>

void print_buffer(const char *data, size_t length)
{
	printf("{ ");
	if (data == NULL) {
		printf("NULL");
	} else {
		for (size_t i = 0; i < length; i++) {
			if (i < length-1)
				printf("0x%02hhx, ", data[i]);
			else
				printf("0x%02hhx", data[i]);
		}
	}
	printf(" }\n");
}

void print_msg(const struct midi_message *msg)
{
	switch (msg->type) {
	case MIDI_TYPE_NOTE_ON:
		printf("NOTE_ON: ch=%u, note=%u, velocity=%u\n", msg->channel,
		       msg->data.note_on.note, msg->data.note_on.velocity);
		break;
	case MIDI_TYPE_NOTE_OFF:
		printf("NOTE_OFF: ch=%u, note=%u, velocity=%u\n", msg->channel,
		       msg->data.note_off.note, msg->data.note_off.velocity);
		break;
	case MIDI_TYPE_POLYPHONIC_PRESSURE:
		printf("POLYPHONIC_PRESSURE: ch=%u, note=%u, pressure=%u\n",
		       msg->channel, msg->data.polyphonic_pressure.note,
		       msg->data.polyphonic_pressure.pressure);
		break;
	case MIDI_TYPE_CONTROL_CHANGE:
		printf("CONTROL_CHANGE: ch=%u, controller=%u, value=%u\n",
		       msg->channel, msg->data.control_change.controller,
		       msg->data.control_change.value);
		break;
	case MIDI_TYPE_PROGRAM_CHANGE:
		printf("PROGRAM_CHANGE: ch=%u, program=%u\n", msg->channel,
		       msg->data.program_change.program);
		break;
	case MIDI_TYPE_CHANNEL_PRESSURE:
		printf("CHANNEL_PRESSURE: ch=%u, pressure=%u\n", msg->channel,
		       msg->data.channel_pressure.pressure);
		break;
	case MIDI_TYPE_PITCH_BEND:
		printf("PITCH_BEND: ch=%u, value=%u\n", msg->channel,
		       msg->data.pitch_bend.value);
		break;
	case MIDI_TYPE_TIME_CODE_QUARTER_FRAME:
		printf("TIME_CODE_QUARTER_FRAME: value=%u\n",
		       msg->data.time_code_quarter_frame.value);
		break;
	case MIDI_TYPE_SONG_POSITION:
		printf("SONG_POSITION: position=%u\n",
		       msg->data.song_position.position);
		break;
	case MIDI_TYPE_SONG_SELECT:
		printf("SONG_SELECT: song=%u\n", msg->data.song_select.song);
		break;
	case MIDI_TYPE_TUNE_REQUEST:
		printf("TUNE_REQUEST\n");
		break;
	case MIDI_TYPE_TIMING_CLOCK:
		printf("TIMING_CLOCK\n");
		break;
	case MIDI_TYPE_START:
		printf("START\n");
		break;
	case MIDI_TYPE_CONTINUE:
		printf("CONTINUE\n");
		break;
	case MIDI_TYPE_STOP:
		printf("STOP\n");
		break;
	case MIDI_TYPE_ACTIVE_SENSE:
		printf("ACTIVE_SENSE\n");
		break;
	case MIDI_TYPE_SYSTEM_RESET:
		printf("SYSTEM_RESET\n");
		break;
	case MIDI_TYPE_SYSEX:
		printf("SysEx: ");
		print_buffer(msg->data.sysex.data, msg->data.sysex.length);
		break;
	default:
		printf("UNKNOWN: 0x%02hhx\n", msg->type);
		break;
	}
}
