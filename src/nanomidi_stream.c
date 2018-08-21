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

#include <nanomidi/decoder.h>
#include <nanomidi/encoder.h>
#include <assert.h>
#include <string.h>

static size_t read_buffer(struct midi_istream *stream, void *data, size_t size)
{
	uint8_t *src = (uint8_t *)stream->param;
	stream->param = src+size;

	for (size_t i = 0; i < size; i++)
		((uint8_t *)data)[i] = src[i];

	return size;
}

static size_t write_buffer(struct midi_ostream *stream, const void *data,
			   size_t size)
{
	uint8_t *dst = (uint8_t *)stream->param;
	stream->param = dst+size;

	for (size_t i = 0; i < size; i++)
		dst[i] = ((uint8_t *)data)[i];

	return size;
}

/**
 * Creates an input stream which reads from a buffer.
 *
 * @ingroup decoder
 *
 * The input stream can be used to read a finite number of bytes from
 * a pre-allocated buffer. It can be also used to decode a single message if
 * the function is called right before midi_decode().
 *
 * @param stream        Pointer to the #midi_istream structure to be initialized
 * @param[in] buffer    Pointer to the buffer to be read from
 * @param size          Buffer size (in bytes)
 */
void midi_istream_from_buffer(struct midi_istream *stream, const void *buffer,
			      size_t size)
{
	assert(stream != NULL);
	assert(buffer != NULL);

	memset(stream, 0, sizeof(struct midi_istream));
	stream->read_cb = &read_buffer;
	stream->capacity = size;
	stream->param = (void *)buffer;
}

/**
 * Creates an output stream which writes to a buffer.
 *
 * @ingroup encoder
 *
 * The output stream can be used to write a finite number of messages into
 * a pre-allocated buffer. It can be also used to encode a single message if
 * the function is called right before midi_encode().
 *
 * @param stream        Pointer to the #midi_ostream structure to be initialized
 * @param buffer        Pointer to the buffer to be written to
 * @param size          Buffer size (in bytes)
 */
void midi_ostream_from_buffer(struct midi_ostream *stream, void *buffer,
			      size_t size)
{
	assert(stream != NULL);
	assert(buffer != NULL);

	memset(stream, 0, sizeof(struct midi_ostream));
	stream->write_cb = &write_buffer;
	stream->capacity = size;
	stream->param = buffer;
}
