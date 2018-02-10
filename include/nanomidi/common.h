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

#ifndef NANOMIDI_COMMON_H
#define NANOMIDI_COMMON_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Unlimited capacity of #midi_istream or #midi_ostream */
#define MIDI_STREAM_CAPACITY_UNLIMITED		(SIZE_MAX)

#ifdef __cplusplus
}
#endif

#endif /* NANOMIDI_COMMON_H */
