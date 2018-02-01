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

#include <nanomidi.h>
#include <assert.h>
#include <string.h>

static size_t read_buffer(struct midi_istream *stream, char *data, size_t size)
{
	char *src = (char *)stream->param;
	stream->param = src+size;

	for (size_t i = 0; i < size; i++)
		data[i] = src[i];

	return size;
}

static size_t write_buffer(struct midi_ostream *stream, const char *data,
			   size_t size)
{
	char *dst = (char *)stream->param;
	stream->param = dst+size;

	for (size_t i = 0; i < size; i++)
		dst[i] = data[i];

	return size;
}

/** @brief Create an input stream which reads from a buffer
 *
 * @ingroup decoder
 *
 * The input stream can be used to read a finite number of bytes from
 * a pre-allocated buffer. It can be also used to decode a single message if
 * the function is called right before @ref midi_decode.
 *
 * @param stream Pointer to the @ref midi_istream structure to be initialized
 * @param buffer Pointer to the buffer to be read from
 * @param size Buffer size (in bytes)
 */
void midi_istream_from_buffer(struct midi_istream *stream, char *buffer,
			      size_t size)
{
	assert(stream != NULL);
	assert(buffer != NULL);

	memset(stream, 0, sizeof(struct midi_istream));
	stream->read_cb = &read_buffer;
	stream->capacity = size;
	stream->param = buffer;
}

/** @brief Create an output stream which writes to a buffer
 *
 * @ingroup encoder
 *
 * The output stream can be used to write a finite number of messages into
 * a pre-allocated buffer. It can be also used to encode a single message if
 * the function is called right before @ref midi_encode.
 *
 * @param stream Pointer to the @ref midi_ostream structure to be initialized
 * @param buffer Pointer to the buffer to be written to
 * @param size Buffer size (in bytes)
 */
void midi_ostream_from_buffer(struct midi_ostream *stream, char *buffer,
			      size_t size)
{
	assert(stream != NULL);
	assert(buffer != NULL);

	memset(stream, 0, sizeof(struct midi_ostream));
	stream->write_cb = &write_buffer;
	stream->capacity = size;
	stream->param = buffer;
}
