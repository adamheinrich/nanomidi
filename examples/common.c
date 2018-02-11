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

void sprint_bytes(char *buffer, const void *bytes, size_t length)
{
	int n = sprintf(buffer, "{ ");
	int pos = (n < 0) ? 0 : n;

	if (buffer == NULL) {
		n = sprintf(&buffer[pos], "NULL");
		pos = (n < 0) ? pos : pos+n;
	} else {
		const uint8_t *d = bytes;
		for (size_t i = 0; i < length; i++) {
			char *comma = (i < length-1) ? ", " : "";
			n = sprintf(&buffer[pos], "0x%02hhx%s", d[i], comma);
			pos = (n < 0) ? pos : pos+n;
		}
	}

	sprintf(&buffer[pos], " }");
}

void sprint_msg(char *buffer, const struct midi_message *msg)
{
	int n;

	switch (msg->type) {
	case MIDI_TYPE_NOTE_ON:
		sprintf(buffer, "NOTE_ON: ch=%u, note=%u, velocity=%u",
			msg->channel, msg->data.note_on.note,
			msg->data.note_on.velocity);
		break;
	case MIDI_TYPE_NOTE_OFF:
		sprintf(buffer, "NOTE_OFF: ch=%u, note=%u, velocity=%u",
			msg->channel, msg->data.note_off.note,
			msg->data.note_off.velocity);
		break;
	case MIDI_TYPE_POLYPHONIC_PRESSURE:
		sprintf(buffer,
			"POLYPHONIC_PRESSURE: ch=%u, note=%u, pressure=%u",
			msg->channel, msg->data.polyphonic_pressure.note,
			msg->data.polyphonic_pressure.pressure);
		break;
	case MIDI_TYPE_CONTROL_CHANGE:
		sprintf(buffer,
			"CONTROL_CHANGE: ch=%u, controller=%u, value=%u",
			msg->channel, msg->data.control_change.controller,
			msg->data.control_change.value);
		break;
	case MIDI_TYPE_PROGRAM_CHANGE:
		sprintf(buffer, "PROGRAM_CHANGE: ch=%u, program=%u",
			msg->channel, msg->data.program_change.program);
		break;
	case MIDI_TYPE_CHANNEL_PRESSURE:
		sprintf(buffer, "CHANNEL_PRESSURE: ch=%u, pressure=%u",
			msg->channel, msg->data.channel_pressure.pressure);
		break;
	case MIDI_TYPE_PITCH_BEND:
		sprintf(buffer, "PITCH_BEND: ch=%u, value=%u",
			msg->channel, msg->data.pitch_bend.value);
		break;
	case MIDI_TYPE_TIME_CODE_QUARTER_FRAME:
		sprintf(buffer, "TIME_CODE_QUARTER_FRAME: value=%u",
			msg->data.time_code_quarter_frame.value);
		break;
	case MIDI_TYPE_SONG_POSITION:
		sprintf(buffer, "SONG_POSITION: position=%u",
			msg->data.song_position.position);
		break;
	case MIDI_TYPE_SONG_SELECT:
		sprintf(buffer, "SONG_SELECT: song=%u",
			msg->data.song_select.song);
		break;
	case MIDI_TYPE_TUNE_REQUEST:
		sprintf(buffer, "TUNE_REQUEST");
		break;
	case MIDI_TYPE_TIMING_CLOCK:
		sprintf(buffer, "TIMING_CLOCK");
		break;
	case MIDI_TYPE_START:
		sprintf(buffer, "START");
		break;
	case MIDI_TYPE_CONTINUE:
		sprintf(buffer, "CONTINUE");
		break;
	case MIDI_TYPE_STOP:
		sprintf(buffer, "STOP");
		break;
	case MIDI_TYPE_ACTIVE_SENSE:
		sprintf(buffer, "ACTIVE_SENSE");
		break;
	case MIDI_TYPE_SYSTEM_RESET:
		sprintf(buffer, "SYSTEM_RESET");
		break;
	case MIDI_TYPE_SYSEX:
		n = sprintf(buffer, "SysEx: ");
		if (n >= 0)
			sprint_bytes(&buffer[n], msg->data.sysex.data,
				     msg->data.sysex.length);
		break;
	default:
		sprintf(buffer, "UNKNOWN: 0x%02hhx", msg->type);
		break;
	}
}

void print_bytes(const void *bytes, size_t length)
{
	char str[256];
	sprint_bytes(str, bytes, length);
	printf("%s\n", str);
}

void print_msg(const struct midi_message *msg)
{
	char str[256];
	sprint_msg(str, msg);
	printf("%s\n", str);
}
