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

#ifndef NANOMIDI_MESSAGES_H
#define NANOMIDI_MESSAGES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * MIDI message definitions and data structures
 * @defgroup messages MIDI Messages
 */

/**@{*/

/** MIDI message types */
enum midi_type {
	/** Channel Mode: Note Off */
	MIDI_TYPE_NOTE_OFF = 0x80,
	/** Channel Mode: Note On */
	MIDI_TYPE_NOTE_ON = 0x90,
	/** Channel Mode: Polyphonic Pressure (Aftertouch) */
	MIDI_TYPE_POLYPHONIC_PRESSURE = 0xa0,
	/** Channel Mode: Control Change */
	MIDI_TYPE_CONTROL_CHANGE = 0xb0,
	/** Channel Mode: Program Change */
	MIDI_TYPE_PROGRAM_CHANGE = 0xc0,
	/** Channel Mode: Channel Pressure (Aftertouch) */
	MIDI_TYPE_CHANNEL_PRESSURE = 0xd0,
	/** Channel Mode: Pitch Bend Change */
	MIDI_TYPE_PITCH_BEND = 0xe0,

	/** System Common: MIDI Time Code Quarter Time */
	MIDI_TYPE_TIME_CODE_QUARTER_FRAME = 0xf1,
	/** System Common: Song Position Pointer */
	MIDI_TYPE_SONG_POSITION = 0xf2,
	/** System Common: Song Select */
	MIDI_TYPE_SONG_SELECT = 0xf3,
	/** System Common: Tune Request */
	MIDI_TYPE_TUNE_REQUEST = 0xf6,

	/** System Real Time: Timing Clock */
	MIDI_TYPE_TIMING_CLOCK = 0xf8,
	/** System Real Time: Start */
	MIDI_TYPE_START = 0xfa,
	/** System Real Time: Continue */
	MIDI_TYPE_CONTINUE = 0xfb,
	/** System Real Time: Stop */
	MIDI_TYPE_STOP = 0xfc,
	/** System Real Time: Active Sensing */
	MIDI_TYPE_ACTIVE_SENSE = 0xfe,
	/** System Real Time: System Reset */
	MIDI_TYPE_SYSTEM_RESET = 0xff,

	/** System Exclusive Message (SysEx) */
	MIDI_TYPE_SYSEX = 0xf0,
	/** Alias for #MIDI_TYPE_SYSEX */
	MIDI_TYPE_SYSTEM_EXCLUSIVE = MIDI_TYPE_SYSEX,
};

/** MIDI message data structure */
struct midi_message {
	/** MIDI message type */
	enum midi_type type;
	/** Channel (0-127) for Channel Mode Messages */
	uint8_t channel;

	/** MIDI message data representation */
	union data {
		/** Representation of #MIDI_TYPE_NOTE_ON. If #velocity is set to
		zero, the message will be interpreted as #MIDI_TYPE_NOTE_OFF. */
		struct note_on {
			uint8_t note; /*!< Note code (0-127) */
			uint8_t velocity; /*!< Note velocity (1-127) */
		} note_on;

		/** Representation of #MIDI_TYPE_NOTE_OFF */
		struct note_off {
			uint8_t note; /*!< Note code (0-127) */
			uint8_t velocity; /*!< Note velocity (0-127) */
		} note_off;

		/** Representation of #MIDI_TYPE_POLYPHONIC_PRESSURE */
		struct polyphonic_pressure {
			uint8_t note; /*!< Note code (0-127) */
			uint8_t pressure; /*!< Pressure value (0-127) */
		} polyphonic_pressure;

		/** Representation of #MIDI_TYPE_CONTROL_CHANGE */
		struct control_change {
			uint8_t controller; /*!< Control number (0-127) */
			uint8_t value; /*!< Control value (0-127) */
		} control_change;

		/** Representation of #MIDI_TYPE_PROGRAM_CHANGE */
		struct program_change {
			uint8_t program; /*!< Program number (0-127) */
		} program_change;

		/** Representation of #MIDI_TYPE_CHANNEL_PRESSURE */
		struct channel_pressure {
			uint8_t pressure; /*!< Pressure value (0-127) */
		} channel_pressure;

		/** Representation of #MIDI_TYPE_PITCH_BEND */
		struct pitch_bend {
			uint16_t value; /*!< Pitch bend change value (0-16383)*/
		} pitch_bend;

		/** Representation of #MIDI_TYPE_TIME_CODE_QUARTER_FRAME */
		struct time_code_quarter_frame {
			uint8_t value; /*< Time code value (0-127) */
		} time_code_quarter_frame;

		/** Representation of #MIDI_TYPE_SONG_POSITION */
		struct song_position {
			uint16_t position; /*!<Song position pointer (0-16383)*/
		} song_position;

		/** Representation of #MIDI_TYPE_SONG_SELECT */
		struct song_select {
			uint8_t song; /*!< Song number (0-127) */
		} song_select;

		/**
		 * Representation of #MIDI_TYPE_SYSEX. Both #data and #length
		 * do not contain "SOX" and "EOX" bytes. */
		struct sysex {
			const void *data; /*!< Pointer to SysEx data */
			size_t length; /*!< Length of data in bytes */
		} sysex;
	} data; /*!< MIDI message data representation */
};

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* NANOMIDI_MESSAGES_H */
