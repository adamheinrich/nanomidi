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

#ifndef NANOMIDI_DECODER_H
#define NANOMIDI_DECODER_H

#include <nanomidi/common.h>
#include <nanomidi/messages.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup decoder
 @{ */

/** Buffer for SysEx messages decoding */
struct midi_sysex_buffer {
	/**
	 * Pointer to a optional buffer allocated by the user.
	 *
	 * Can be set to `NULL` if SysEx decoding is not needed. The buffer
	 * should be large enough to accommodate the largest possible SysEx
	 * message (excluding "SOX" and "EOX" bytes).
	 */
	void *data;
	/** Buffer size */
	size_t size;
};

/**
 * Input stream for midi_decode()
 *
 * Read callback read_cb() and stream capacity must be provided by the user.
 * If SysEx decoding is required, it is necessary to provide a buffer
 * in #sysex_buffer. It is possible to use midi_istream_from_buffer() to create
 * a stream which reads from a buffer.
 */
struct midi_istream {
	/**
	 * Pointer to a user-implemented read callback. The callback should
	 * read the exact number of bytes requested.
	 *
	 * @param stream        Pointer to associated #midi_istream
	 * @param[out] data     Data read
	 * @param size          Number of bytes to be read
	 *
	 * @returns The number of bytes actually read
	 */
	size_t (*read_cb)(struct midi_istream *stream, void *data, size_t size);
	/**
	 * Stream capacity. Function midi_decode() will not read more than
	 * `capacity` bytes from the stream unless midi_istream.capacity is set
	 * to #MIDI_STREAM_CAPACITY_UNLIMITED.
	 */
	size_t capacity;
	/**
	 * Message data structure filled by the decoder. Once a message is
	 * decoded, midi_decode() returns pointer to midi_istream.msg.
	 */
	struct midi_message msg;
	/**
	 * Message data structure filled by the decoder.
	 * Once a System Real Time Message is decoded, midi_decode() returns
	 * pointer to midi_istream.rtmsg.
	 */
	struct midi_message rtmsg;
	/** Buffer for SysEx messages decoding */
	struct midi_sysex_buffer sysex_buffer;
	/** Number of bytes remaining to complete the current message
	(handled internally). */
	int bytes_left;
	/** Optional parameter to be passed to read_cb() */
	void *param;
};

void midi_istream_from_buffer(struct midi_istream *stream, const void *buffer,
			      size_t size);
struct midi_message *midi_decode(struct midi_istream *stream);
struct midi_message *midi_decode_usb(struct midi_istream *stream,
				     uint8_t *cable_number);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* NANOMIDI_DECODER_H */
