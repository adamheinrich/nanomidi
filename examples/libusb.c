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
#include "common.h"

struct endpoint_table {
	uint8_t *addresses;
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

				/* Save MIDI IN endpoints to table: */
				uint8_t ep, eid;
				for (eid = 0; eid < id->bNumEndpoints; eid++) {
					ep = id->endpoint[eid].bEndpointAddress;
					if ((ep & LIBUSB_ENDPOINT_IN) != 0) {
						size_t pos = ep_table->length;
						if (pos > ep_table->max_size)
							break;

						ep_table->addresses[pos] = ep;
						ep_table->length++;
					}
				}
			}
		}

		libusb_free_config_descriptor(config);
	}

	return true;
}

static void midi_run(libusb_device_handle *devh,
		     struct endpoint_table *ep_table)
{
	uint8_t data[4];
	int ret;
	int length;

	struct midi_istream istream;

	while (!stop) {
		/* Bulk read from all available IN endpoints: */
		for (size_t i = 0; i < ep_table->length; i++) {
			ret = libusb_bulk_transfer(devh, ep_table->addresses[i],
						   data, sizeof(data),
						   &length, 100);

			/* First byte contains Cable Index Number: */
			uint8_t cin = (data[0] & 0x0f);
			if (ret < 0 || length < 4 || cin < 0x08)
				continue;

			/* Reset stream and encode MIDI message: */
			midi_istream_from_buffer(&istream, &data[1], 3);
			struct midi_message *m = midi_decode(&istream);
			if (m != NULL)
				print_msg(m);
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

	uint8_t addresses[16];
	struct endpoint_table ep_table = {
		.addresses = addresses,
		.max_size = sizeof(addresses),
	};

	devh = libusb_open_device_with_vid_pid(NULL, vid, pid);
	if (devh) {
		if (!midi_init(devh, &ep_table))
			fprintf(stderr, "Error initializing USB device\n");

		if (ep_table.length == 0)
			fprintf(stderr, "No MIDI IN endpoints found\n");
		else
			midi_run(devh, &ep_table);

		libusb_close(devh);
	} else {
		fprintf(stderr, "Error opening USB device\n");
	}

	libusb_exit(NULL);
	return (ep_table.length > 0) ? 0 : 1;
}
