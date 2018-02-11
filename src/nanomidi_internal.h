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

#ifndef NANOMIDI_INTERNAL_H
#define NANOMIDI_INTERNAL_H

#include <nanomidi/messages.h>

#define DATA_BYTE(data)		((data) & 0x7f)
#define MIDI_TYPE_SYSTEM_BASE	MIDI_TYPE_SYSEX

enum midi_type_sysex {
	MIDI_TYPE_SOX = MIDI_TYPE_SYSEX,
	MIDI_TYPE_EOX = 0xf7,
};

#endif /* NANOMIDI_INTERNAL_H */
