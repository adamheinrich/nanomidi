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

#include <nanomidi_arduino.h>

midi_ostream ostream;
midi_message message;
bool note_on;

size_t write_midi(midi_ostream *stream, const void *data, size_t len)
{
	return Serial.write((const uint8_t *)data, len);
}

void setup()
{
	/* Configure the on-board LED: */
	pinMode(LED_BUILTIN, OUTPUT);

	/* Configure the output stream to call write_midi() whenever it has any
	   bytes to send out: */
	ostream.write_cb = &write_midi;
	ostream.capacity = MIDI_STREAM_CAPACITY_UNLIMITED;

	/* Use the standard baudrate: */
	Serial.begin(31250);
}

void loop()
{
	note_on = !note_on;

	if (note_on) {
		message.type = MIDI_TYPE_NOTE_ON;
		message.channel = 0;
		message.data.note_on.note = 60; /* Note C4 */
		message.data.note_on.velocity = 127;
	} else {
		message.type = MIDI_TYPE_NOTE_OFF;
		message.channel = 0;
		message.data.note_off.note = 60;
		message.data.note_off.velocity = 0;
	}

	midi_encode(&ostream, &message);

	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);

	if (note_on)
		delay(200);
	else
		delay(900);
}
