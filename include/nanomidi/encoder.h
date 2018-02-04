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

#ifndef NANOMIDI_ENCODER_H
#define NANOMIDI_ENCODER_H

#include <nanomidi/common.h>
#include <nanomidi/messages.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Output stream for @ref midi_encode
 *
 * Write callback @ref write_cb and stream @ref capacity must be provided by
 * the user.
 *
 * Alternatively, it is possible to use @ref midi_ostream_from_buffer to create
 * a stream which writes to a buffer.
 *
 * @ingroup encoder
 */
struct midi_ostream {
	/** @brief Pointer to a user-implemented write callback
	 *
	 * The callback should write the exact number of bytes requested.
	 *
	 * @param stream Pointer to @ref midi_ostream associated with the
	 * callback.
	 * @param[in] data Data to be written
	 * @param size Number of bytes to be written
	 *
	 * @returns The number of bytes actually written
	 */
	size_t (*write_cb)(struct midi_ostream *stream, const char *data,
			   size_t size);
	/** @brief Stream capacity (@ref MIDI_STREAM_CAPACITY_UNLIMITED for
	 * unlimited capacity)
	 *
	 * Functon @ref midi_encode will not write more than `capacity` bytes
	 * to the stream if `capacity` is not set to
	 * @ref MIDI_STREAM_CAPACITY_UNLIMITED. */
	size_t capacity;
	/** @brief Optional parameter to be passed to @ref write_cb */
	void *param;
};

void midi_ostream_from_buffer(struct midi_ostream *stream, char *buffer,
			      size_t size);
size_t midi_encode(struct midi_ostream *stream, const struct midi_message *msg);

#ifdef __cplusplus
}
#endif

#endif /* NANOMIDI_ENCODER_H */
