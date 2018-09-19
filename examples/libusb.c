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

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <libusb.h>
#include <nanomidi/decoder.h>
#include <nanomidi/encoder.h>
#include "common.h"

enum endpoint_direction {
	EP_IN,
	EP_OUT,
};

struct endpoint_table {
	struct endpoint {
		uint8_t address;
		enum endpoint_direction direction;
	} *endpoints;
	size_t length;
	size_t max_size;
};

static volatile sig_atomic_t stop;

static void sig_handler(int signum) {
	(void)signum;
	stop = 1;
}

static bool midi_init(libusb_device_handle *devh,
		      struct endpoint_table *ep_table)
{
	int ret;
	libusb_device *dev = libusb_get_device(devh);

	ret = libusb_set_auto_detach_kernel_driver(devh, 1);
	if (ret < 0)
		return false;

	/* Get device descriptor: */
	struct libusb_device_descriptor desc;
	ret = libusb_get_device_descriptor(dev, &desc);
	if (ret < 0) {
		fprintf(stderr, "Failed to get device descriptor\n");
		return false;
	}

	/* Iterate over all interfaces: */
	for (uint8_t cfg_id = 0; cfg_id < desc.bNumConfigurations; cfg_id++) {
		struct libusb_config_descriptor *config;
		ret = libusb_get_config_descriptor(dev, cfg_id, &config);
		if (ret < 0) {
			fprintf(stderr, "Failed to get config descriptor\n");
			continue;
		}

		const struct libusb_interface *it;
		const struct libusb_interface_descriptor *id;
		for (uint8_t i = 0; i < config->bNumInterfaces; i++) {
			it = &config->interface[i];
			for (int j = 0; j < it->num_altsetting; j++) {
				id = &it->altsetting[j];

				/* Check for MIDI interface: */
				if (id->bInterfaceClass != LIBUSB_CLASS_AUDIO ||
				    id->bInterfaceSubClass != 3) {
					continue;
				}

				/* Claim interface: */
				uint8_t if_num = id->bInterfaceNumber;
				ret = libusb_claim_interface(devh, if_num);
				if (ret < 0) {
					fprintf(stderr,
						"Cannot claim interface %u\n",
						if_num);
					libusb_free_config_descriptor(config);
					return false;
				}

				/* Save MIDI IN and OUT endpoints to table: */
				uint8_t ep, eid;
				for (eid = 0; eid < id->bNumEndpoints; eid++) {
					ep = id->endpoint[eid].bEndpointAddress;

					size_t pos = ep_table->length;
					if (pos > ep_table->max_size)
						break;

					if ((ep & LIBUSB_ENDPOINT_IN)) {
						ep_table->endpoints[pos] =
						(struct endpoint) {
							.address = ep,
							.direction = EP_IN,
						};
						ep_table->length++;
					} else {
						ep_table->endpoints[pos] =
						(struct endpoint) {
							.address = ep,
							.direction = EP_OUT,
						};
						ep_table->length++;
					}
				}
			}
		}

		libusb_free_config_descriptor(config);
	}

	return true;
}

static void sysex_identity_request(libusb_device_handle *devh,
				   struct endpoint_table *ep_table)
{
	uint8_t buffer[64];
	int length;

	uint8_t id_request[] = { 0x7e, 0x7f, 0x06, 0x01 };

	struct midi_message msg = {
		.type = MIDI_TYPE_SYSEX,
		.data.sysex.data = id_request,
		.data.sysex.length = sizeof(id_request),
	};

	for (size_t i = 0; i < ep_table->length; i++) {
		struct endpoint *ep = &ep_table->endpoints[i];
		if (ep->direction != EP_OUT)
			continue;

		struct midi_ostream ostream = { 0 };
		midi_ostream_from_buffer(&ostream, buffer, sizeof(buffer));
		int enc_len = (int)midi_encode_usb(&ostream, &msg, 0);

		if (enc_len > 0) {
			libusb_bulk_transfer(devh, ep->address, buffer, enc_len,
					     &length, 100);
		}
	}
}

static void midi_run(libusb_device_handle *devh,
		     struct endpoint_table *ep_table)
{
	uint8_t buffer[64];
	int ret;
	int length;
	uint8_t cable_number;

	struct midi_istream istream;
	midi_istream_from_buffer(&istream, buffer, sizeof(buffer));

	char sysex_buffer[64];
	istream.sysex_buffer.data = sysex_buffer;
	istream.sysex_buffer.size = sizeof(sysex_buffer);

	sysex_identity_request(devh, ep_table);

	while (!stop) {
		/* Bulk read from all available IN endpoints: */
		for (size_t i = 0; i < ep_table->length; i++) {
			struct endpoint *ep = &ep_table->endpoints[i];
			if (ep->direction != EP_IN)
				continue;

			ret = libusb_bulk_transfer(devh, ep->address,
						   buffer, sizeof(buffer),
						   &length, 100);
			if (ret != 0)
				continue;

			/* Reset stream and encode MIDI message: */
			istream.param = buffer;
			istream.capacity = (size_t)length;

			struct midi_message *msg;
			do {
				msg = midi_decode_usb(&istream, &cable_number);
				if (msg != NULL)
					print_msg(msg);
			} while (msg != NULL);
		}
	}
}

int main(int argc, char **argv)
{
	struct libusb_device_handle *devh;
	uint16_t vid, pid;

	signal(SIGINT, sig_handler);

	if (argc != 2 || sscanf(argv[1], "%04hx:%04hx", &vid, &pid) != 2) {
		fprintf(stderr, "Usage: %s vid:pid\n", argv[0]);
		fprintf(stderr, "Run `lsusb` to get VID and PID pair\n");
		return 1;
	}

	int ret = libusb_init(NULL);
	if (ret < 0) {
		fprintf(stderr, "Error initializing libusb\n");
		return 1;
	}

	struct endpoint endpoints[16];
	struct endpoint_table ep_table = {
		.endpoints = endpoints,
		.max_size = sizeof(endpoints)/sizeof(*endpoints),
	};

	devh = libusb_open_device_with_vid_pid(NULL, vid, pid);
	if (devh) {
		if (!midi_init(devh, &ep_table))
			fprintf(stderr, "Error initializing USB device\n");

		if (ep_table.length == 0)
			fprintf(stderr, "No MIDI endpoints found\n");
		else
			midi_run(devh, &ep_table);

		libusb_close(devh);
	} else {
		fprintf(stderr, "Error opening USB device\n");
	}

	libusb_exit(NULL);
	return (ep_table.length > 0) ? 0 : 1;
}
