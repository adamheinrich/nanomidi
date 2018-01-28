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

#define DATA_BYTE(data)		((char)((data) & 0x7f))

enum midi_type_system {
	MIDI_TYPE_SYSTEM_BASE = 0xf0,
	MIDI_TYPE_SOX = 0xf1,
	MIDI_TYPE_EOX = 0xf7,
};

#endif /* NANOMIDI_INTERNAL_H */
