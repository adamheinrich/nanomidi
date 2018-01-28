# nanomidi

Nanomidi is a simple stream-based MIDI message encoder and decoder written in C.
It is suitable for embedded (microcontroller) applications.

## Scope

Implemented:

 - Message encoder `midi_encode()`
 - Message decoder `midi_decode()` with support for **Running Status** (omitted
   status byte in successive messages of the same type)
 - Data structures for **Channel Mode Messages** (Note On, Control Change, etc.)
 - Data structures for **System Common Messages**
 - Support for **System Real Time Messages** (single-byte messages which can
   occur anywhere in the stream

Not implemented:

 - Support for System Exclusive Messages

## License

Nanomidi is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

See file `COPYING` for details.
