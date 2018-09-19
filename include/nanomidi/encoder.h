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

/** @addtogroup encoder
 @{ */

/**
 * Output stream for midi_encode()
 *
 * Write callback write_cb() and stream capacity must be provided by the user.
 * It is also possible to call midi_ostream_from_buffer() to create a stream
 * which writes to a buffer.
 */
struct midi_ostream {
	/**
	 * Pointer to a user-implemented write callback. The callback should
	 * write the exact number of bytes requested.
	 *
	 * @param stream        Pointer to associated #midi_ostream
	 * @param[in] data      Data to be written
	 * @param size          Number of bytes to be written
	 *
	 * @returns The number of bytes actually written
	 */
	size_t (*write_cb)(struct midi_ostream *stream, const void *data,
			   size_t size);
	/**
	 * Stream capacity. Function midi_encode() will not write more than
	 * `capacity` bytes to the stream unless midi_ostream.capacity is set
	 * to #MIDI_STREAM_CAPACITY_UNLIMITED.
	 */
	size_t capacity;
	/** Optional parameter to be passed to write_cb() */
	void *param;
};

void midi_ostream_from_buffer(struct midi_ostream *stream, void *buffer,
			      size_t size);
size_t midi_encode(struct midi_ostream *stream, const struct midi_message *msg);
size_t midi_encode_usb(struct midi_ostream *stream,
		       const struct midi_message *msg, uint8_t cable_number);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* NANOMIDI_ENCODER_H */
