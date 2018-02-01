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

#ifndef NANOMIDI_H
#define NANOMIDI_H

#include <nanomidi_messages.h>

/** @brief Unlimited capacity of @ref midi_ostream
 * @ingroup decoder */
#define MIDI_OSTREAM_CAPACITY_UNLIMITED		SIZE_MAX

/** @brief Buffer for SysEx messages decoding
 * @ingroup decoder
 */
struct midi_sysex_buffer {
	/** @brief Pointer to a optional buffer allocated by the user
	 *
	 * Can be set to `NULL` if SysEx decoding is not needed. The buffer
	 * should be large enough to accommodate the largest possible SysEx
	 * message (excluding "SOX" and "EOX" bytes). */
	char *data;
	/** @brief Buffer size */
	size_t size;
};

/** @brief Input stream
 * @ingroup decoder
 */
struct midi_istream {
	/** @brief Pointer to a user-implemented read callback
	 *
	 * The callback should read the exact number of bytes requested.
	 *
	 * @param stream Pointer to @ref midi_istream associated with the
	 * callback.
	 * @param[out] data Data read
	 * @param size Number of bytes to be read
	 *
	 * @returns The number of bytes actually read
	 */
	size_t (*read_cb)(struct midi_istream *stream, char *data, size_t size);
	/** @brief Message data structure filled by the decoder.
	 *
	 * Once a message is decoded, @ref midi_decode returns a pointer
	 * to @ref msg. */
	struct midi_message msg;
	/** @brief Message data structure filled by the decoder.
	 *
	 * Once a System Real Time Message is decoded, @ref midi_decode returns
	 * a pointer to @ref rtmsg. */
	struct midi_message rtmsg;
	/** @brief Buffer for SysEx messages decoding */
	struct midi_sysex_buffer sysex_buffer;
	/** @brief Number of bytes remaining to complete the current message
	    (handled internally). */
	int bytes_left;
	/** @brief Optional parameter to be passed to @ref read_cb */
	void *param;
};

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
	/** @brief Stream capacity (@ref MIDI_OSTREAM_CAPACITY_UNLIMITED for
	 * unlimited capacity)
	 *
	 * Functon @ref midi_encode will not write more than `capacity` bytes
	 * to the stream if `capacity` is not set to
	 * @ref MIDI_OSTREAM_CAPACITY_UNLIMITED. */
	size_t capacity;
	/** @brief Optional parameter to be passed to @ref write_cb */
	void *param;
};

void midi_ostream_from_buffer(struct midi_ostream *stream, char *buffer,
			      size_t size);

struct midi_message *midi_decode(struct midi_istream *stream);
size_t midi_encode(struct midi_ostream *stream, const struct midi_message *msg);

#endif /* NANOMIDI_H */
