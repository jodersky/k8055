/* k8055 driver for libusb-1.0

 Copyright (c) 2012 by Jakob Odersky
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Thanks to the following people who wrote the original version of `libk8055'
 (http://libk8055.sourceforge.net/):

 2005 by Sven Lindberg <k8055@k8055.mine.nu>

 2007 by Pjetur G. Hjaltason <pjetur@pjetur.net>
 Commenting, general rearrangement of code, bugfixes,
 python interface with swig and simple k8055 python class

 The comments explaining the data packets and debounce time conversion are from them.

 Input packet format

 +---+---+---+---+---+---+---+---+
 |DIn|Sta|A1 |A2 |   C1  |   C2  |
 +---+---+---+---+---+---+---+---+
 DIn = Digital input in high nibble, except for input 3 in 0x01
 Sta = Status, Board number + 1
 A1  = Analog input 1, 0-255
 A2  = Analog input 2, 0-255
 C1  = Counter 1, 16 bits (lsb)
 C2  = Counter 2, 16 bits (lsb)

 Output packet format


 +---+---+---+---+---+---+---+---+
 |CMD|DIG|An1|An2|Rs1|Rs2|Dbv|Dbv|
 +---+---+---+---+---+---+---+---+
 CMD = Command
 DIG = Digital output bitmask
 An1 = Analog output 1 value, 0-255
 An2 = Analog output 2 value, 0-255
 Rs1 = Reset counter 1, command 3
 Rs2 = Reset counter 3, command 4
 Dbv = Debounce value for counter 1 and 2, command 1 and 2

 Or split by commands

 Cmd 0, Reset ??
 Cmd 1, Set debounce Counter 1
 +---+---+---+---+---+---+---+---+
 |CMD|   |   |   |   |   |Dbv|   |
 +---+---+---+---+---+---+---+---+
 Cmd 2, Set debounce Counter 2
 +---+---+---+---+---+---+---+---+
 |CMD|   |   |   |   |   |   |Dbv|
 +---+---+---+---+---+---+---+---+
 Cmd 3, Reset counter 1
 +---+---+---+---+---+---+---+---+
 | 3 |   |   |   | 00|   |   |   |
 +---+---+---+---+---+---+---+---+
 Cmd 4, Reset counter 2
 +---+---+---+---+---+---+---+---+
 | 4 |   |   |   |   | 00|   |   |
 +---+---+---+---+---+---+---+---+
 cmd 5, Set analog/digital
 +---+---+---+---+---+---+---+---+
 | 5 |DIG|An1|An2|   |   |   |   |
 +---+---+---+---+---+---+---+---+

*/


#define PACKET_LENGTH 8
#define K8055_PRODUCT_ID 0x5500
#define VELLEMAN_VENDOR_ID 0x10cf
#define K8055_MAX_DEVICES 4

#define USB_OUT_EP 0x01	/** USB output endpoint */
#define USB_IN_EP 0x81 /* USB Input endpoint */
#define USB_TIMEOUT 20 /* [ms] */

#define IN_DIGITAL_OFFSET 0
#define IN_ANALOG_1_OFFSET 2
#define IN_ANALOG_2_OFFSET 3
#define IN_COUNTER_1_OFFSET 4
#define IN_COUNTER_2_OFFSET 6

#define OUT_DIGITAL_OFFSET 1
#define OUT_ANALOG_1_OFFSET 2
#define OUT_ANALOG_2_OFFSET 3
#define OUT_COUNTER_1_OFFSET 4
#define OUT_COUNTER_2_OFFSET 5
#define OUT_COUNTER_1_DEBOUNCE_OFFSET 6
#define OUT_COUNTER_2_DEBOUNCE_OFFSET 7

#define CMD_RESET 0
#define CMD_SET_DEBOUNCE_1 1
#define CMD_SET_DEBOUNCE_2 2
#define CMD_RESET_COUNTER_1 3
#define CMD_RESET_COUNTER_2 4
#define CMD_SET_ANALOG_DIGITAL 5

#include <libusb.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "k8055.h"

/** Represents a Vellemean K8055 USB board. */
typedef struct k8055_device {
    
    /** Data last read from device, used by read_data(). */
    unsigned char data_in[PACKET_LENGTH];

    /** Data to be sent to the device, used by write_data(). */
    unsigned char data_out[PACKET_LENGTH];

    /** Underlying libusb handle to device. NULL if the device is not open. */
    libusb_device_handle *device_handle;
} k8055_device;


/* Global Variables
* ================
* Yes, unfortunately. However, as this program is meant to be a driver,
* access to these variables from the outside should not occur. */

/** Libusb context. */
static libusb_context* context = NULL;

/** This array contains all K8055 devices, regardless of their connection status. The index corresponds to a device's port. */
static k8055_device devices[K8055_MAX_DEVICES];

static int DEBUG = 0;

/* end global variables */


/** Prints the given message to standard output if debugging is enabled. */
static void print_error(const char * str) {
	if (DEBUG) {
		printf("%s", str);
	}
}

/** Retrieves the number of open devices. Internally, a device is open if it's libusb device handle is not null. */
int k8055_count_open_devices() {
	int r = 0;
	for (int i = 0; i < K8055_MAX_DEVICES; ++i) {
		if (devices[i].device_handle != NULL) r += 1;
	}
	return r;
}


int k8055_open_device(int port) {
	if (k8055_count_open_devices() == 0) { /* no devices are open */
		int r = libusb_init(&context); /* initialize a new context */
		if (r < 0) {
			print_error("could not initialize libusb");
			return K8055_ERROR_INIT_LIBUSB; /* return error code in case of error */
		}

	}

	libusb_device **connected_devices = NULL;

	ssize_t size = libusb_get_device_list(context, &connected_devices); /* get all devices on system */
	if (size <= 0) {
		print_error("no usb devices found on system");
		return K8055_ERROR_NO_DEVICES;
	}

	libusb_device *k8055 = NULL; /* device on port */

	for(size_t i = 0; i < size; ++i) { /* look for the device at given port */
		struct libusb_device_descriptor descriptor;
		libusb_get_device_descriptor(connected_devices[i], &descriptor);
		if (descriptor.idVendor == VELLEMAN_VENDOR_ID && descriptor.idProduct == (K8055_PRODUCT_ID + port))
			k8055 = connected_devices[i];
	}
	if (k8055 == NULL) {
		print_error("velleman k8055 not found at port");
		return K8055_ERROR_NO_K8055;
	}

	libusb_device_handle *handle = NULL; /* handle to device on port */

	int r = libusb_open(k8055, &handle); /* open device */
	libusb_free_device_list(connected_devices, 1); /* we got the handle, free references to other devices */

	if (r == LIBUSB_ERROR_ACCESS) {
		print_error("could not open device, you don't have the required permissions");
		return K8055_ERROR_ACCESS;
	} else if (r != 0) {
		print_error("could not open device");
		return K8055_ERROR_OPEN;
	}

	if(libusb_kernel_driver_active(handle, 0) == 1) { /* find out if kernel driver is attached */
		if(libusb_detach_kernel_driver(handle, 0) != 0) { /* detach it */
			print_error("could not detach kernel driver");
			return K8055_ERROR_OPEN;
		}
	}

	r = libusb_claim_interface(handle, 0); /* claim interface 0 (the first) of device */
	if (r != 0) {
		print_error("could not claim interface");
		return K8055_ERROR_OPEN;
	}

	devices[port].device_handle = handle; /* mark device as open by assigning it a libusb device handle */

	return 0;
}

void k8055_close_device(int port) {
	if (devices[port].device_handle == NULL) /* already closed */
		return;

	libusb_release_interface(devices[port].device_handle, 0);
	libusb_close(devices[port].device_handle);
	devices[port].device_handle = NULL;
	if (k8055_count_open_devices() == 0) libusb_exit(context);
}

/** Writes the actual contained in the device's data_out field to the usb endpoint.
 * @return K8055_ERROR_INDEX if port is an invalid index
 * @return K8055_ERROR_CLOSED if the board associated to the given port is not open
 * @return K8055_ERROR_WRITE if another error occurred during the write process */
static int write_data(int port) {
    int write_status = 0;

    if (port < 0 || K8055_MAX_DEVICES <= port) {
    	print_error("invalid port number, port p should be 0<=p<=3");
    	return K8055_ERROR_INDEX;
    }

    if (devices[port].device_handle == 0) {
    	print_error("unable to write to port, device not open");
    	return K8055_ERROR_CLOSED;
    }

    k8055_device *device = &devices[port];

    int transferred = 0;
    for(int i=0; i < 3; ++i) {
    	write_status = libusb_interrupt_transfer(
    			device->device_handle,
    			USB_OUT_EP,
    			(unsigned char *) device->data_out,
    			PACKET_LENGTH,
    			&transferred,
    			USB_TIMEOUT);
        if (write_status == 0 && transferred == PACKET_LENGTH) break;
    }
    if (write_status != 0 || transferred != PACKET_LENGTH) {
    	print_error("could not write packet");
    	return K8055_ERROR_WRITE;
    }
    return 0;
}

/** Reads data from the usb endpoint into the device's data_in field.
 * @return K8055_ERROR_INDEX if port is an invalid index
 * @return K8055_ERROR_CLOSED if the board associated to the given port is not open
 * @return K8055_ERROR_READ if another error occurred during the read process */
static int read_data(int port, int cycles) {
    int read_status = 0;

    if (port < 0 || K8055_MAX_DEVICES <= port) {
		print_error("invalid port number, port p should be 0<=p<=3");
		return K8055_ERROR_INDEX;
	}

    if (devices[port].device_handle == 0) {
        	print_error("unable to read from port, device not open");
        	return K8055_ERROR_CLOSED;
    }

    k8055_device *device = &devices[port];

    int transferred = 0;
    for(int i = 0; i < 3; ++i) {
    	for(int j = 0; j < cycles; ++j) { /* read twice to get fresh data, (i.e. circumvent some kind of buffer) */
    		read_status = libusb_interrupt_transfer(
    				device->device_handle,
    				USB_IN_EP,
    				(unsigned char *) device->data_in,
    				PACKET_LENGTH,
    				&transferred,
    				USB_TIMEOUT);
    	}
        if (read_status == 0 && transferred == PACKET_LENGTH) break;
    }
	if (read_status != 0 || transferred != PACKET_LENGTH) {
		print_error("could not read packet");
		return K8055_ERROR_READ;
	}
	return 0;
}

static int intToDebounce(int x) {
	int t = x;
	/* the velleman k8055 use a exponetial formula to split up the
	 DebounceTime 0-7450 over value 1-255. I've tested every value and
	 found that the formula dbt=0,338*value^1,8017 is closest to
	 vellemans dll. By testing and measuring times on the other hand I
	 found the formula dbt=0,115*x^2 quite near the actual values, a
	 little below at really low values and a little above at really
	 high values. But the time set with this formula is within +-4% */
	if (t > 7450)
		t = 7450;
	t = sqrt(t / 0.115);
	if (t > ((int) t + 0.49999999)) /* simple round() function) */
		t += 1;
	return t;
}

int k8055_set_all_digital(int port, int bitmask) {
	k8055_device *device = &devices[port];
	device->data_out[OUT_DIGITAL_OFFSET] = bitmask;
	device->data_out[0] = CMD_SET_ANALOG_DIGITAL;
	return write_data(port);
}

int k8055_set_digital(int port, int channel, int value) {
	k8055_device *device = &devices[port];

	unsigned char data = device->data_out[OUT_DIGITAL_OFFSET];
	if (value == 0) /* off */
		data = data & ~(1 << channel);
	else /* on */
		data = data | (1 << channel);

	device->data_out[OUT_DIGITAL_OFFSET] = data;
	device->data_out[0] = CMD_SET_ANALOG_DIGITAL;
	return write_data(port);
}

int k8055_set_all_analog(int port, int analog1, int analog2) {
	k8055_device *device = &devices[port];
	device->data_out[OUT_ANALOG_1_OFFSET] = analog1;
	device->data_out[OUT_ANALOG_2_OFFSET] = analog2;
	device->data_out[0] = CMD_SET_ANALOG_DIGITAL;
	return write_data(port);
}

int k8055_set_analog(int port, int channel, int value) {
	k8055_device *device = &devices[port];

	if (channel == 0) {
		device->data_out[OUT_ANALOG_1_OFFSET] = value;
	} else if (channel == 1) {
		device->data_out[OUT_ANALOG_2_OFFSET] = value;
	} else {
		print_error("can't write to unknown analog port");
		return K8055_ERROR_INDEX;
	}

	device->data_out[0] = CMD_SET_ANALOG_DIGITAL;
	return write_data(port);
}

int k8055_reset_counter(int port, int counter) {
	k8055_device *device = &devices[port];

	if (counter == 0) {
		device->data_out[OUT_COUNTER_1_OFFSET] = 0;
		device->data_out[0] = CMD_RESET_COUNTER_1;
	} else if (counter == 1) {
		device->data_out[OUT_COUNTER_2_OFFSET] = 0;
		device->data_out[0] = CMD_RESET_COUNTER_2;
	} else {
		print_error("can't reset unknown counter");
		return K8055_ERROR_INDEX;
	}

	return write_data(port);
}

int k8055_set_debounce_time(int port, int counter, int debounce) {
	k8055_device *device = &devices[port];

	if (counter == 0) {
		device->data_out[OUT_COUNTER_1_DEBOUNCE_OFFSET] = intToDebounce(debounce);
		device->data_out[0] = CMD_SET_DEBOUNCE_1;
	} else if (counter == 1) {
		device->data_out[OUT_COUNTER_2_DEBOUNCE_OFFSET] = intToDebounce(debounce);
		device->data_out[0] = CMD_SET_DEBOUNCE_2;
	} else {
		print_error("can't set debounce time for unknown counter");
		return K8055_ERROR_INDEX;
	}

	return write_data(port);
}

static int get_all_cycle(int port, int *digitalBitmask, int *analog1, int *analog2, int *counter1, int *counter2, int cycles) {
	int r = read_data(port, cycles);
	if (r != 0) return r;

	k8055_device *device = &devices[port];

	if (digitalBitmask != NULL)
		*digitalBitmask = (
				((device->data_in[IN_DIGITAL_OFFSET] >> 4) & 0x03) | /* Input 1 and 2 */
				((device->data_in[IN_DIGITAL_OFFSET] << 2) & 0x04) | /* Input 3 */
				((device->data_in[IN_DIGITAL_OFFSET] >> 3) & 0x18)); /* Input 4 and 5 */
	if (analog1 != NULL)
		*analog1 = device->data_in[IN_ANALOG_1_OFFSET];
	if (analog2 != NULL)
		*analog2 = device->data_in[IN_ANALOG_2_OFFSET];
	if (counter1 != NULL)
		*counter1 = (int) device->data_in[IN_COUNTER_1_OFFSET + 1] << 8 | device->data_in[IN_COUNTER_1_OFFSET];
	if (counter2 != NULL)
		*counter2 = (int) device->data_in[IN_COUNTER_2_OFFSET + 1] << 8 | device->data_in[IN_COUNTER_2_OFFSET];
	return 0;
}

int k8055_get_all(int port, int *digitalBitmask, int *analog1, int *analog2, int *counter1, int *counter2) {
	return get_all_cycle(port, digitalBitmask, analog1, analog2, counter1, counter2, 2);
}

int k8055_quick_get_all(int port, int *digitalBitmask, int *analog1, int *analog2, int *counter1, int *counter2) {
	return getAllCycle(port, digitalBitmask, analog1, analog2, counter1, counter2, 1);
}
