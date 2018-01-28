#include "common.h"
#include <stdio.h>

void print_msg(const struct midi_message *msg)
{
	switch (msg->status) {
	case MIDI_STATUS_NOTE_ON:
		printf("NOTE_ON: ch=%u, note=%u, velocity=%u\n", msg->channel,
		       msg->data.note_on.note, msg->data.note_on.velocity);
		break;
	case MIDI_STATUS_NOTE_OFF:
		printf("NOTE_OFF: ch=%u, note=%u, velocity=%u\n", msg->channel,
		       msg->data.note_off.note, msg->data.note_off.velocity);
		break;
	case MIDI_STATUS_POLYPHONIC_PRESSURE:
		printf("POLYPHONIC_PRESSURE: ch=%u, note=%u, pressure=%u\n",
		       msg->channel, msg->data.polyphonic_pressure.note,
		       msg->data.polyphonic_pressure.pressure);
		break;
	case MIDI_STATUS_CONTROL_CHANGE:
		printf("CONTROL_CHANGE: ch=%u, controller=%u, value=%u\n",
		       msg->channel, msg->data.control_change.controller,
		       msg->data.control_change.value);
		break;
	case MIDI_STATUS_PROGRAM_CHANGE:
		printf("PROGRAM_CHANGE: ch=%u, program=%u\n", msg->channel,
		       msg->data.program_change.program);
		break;
	case MIDI_STATUS_CHANNEL_PRESSURE:
		printf("CHANNEL_PRESSURE: ch=%u, pressure=%u\n", msg->channel,
		       msg->data.channel_pressure.pressure);
		break;
	case MIDI_STATUS_PITCH_BEND:
		printf("PITCH_BEND: ch=%u, value=%u\n", msg->channel,
		       msg->data.pitch_bend.value);
		break;
	default:
		printf("UNKNOWN: %u\n", msg->status);
		break;
	}
}
