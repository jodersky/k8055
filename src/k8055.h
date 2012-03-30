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
 */

#ifndef K8055_H_
#define K8055_H_

enum ErrorCode {
	K8055_SUCCESS = 0,
	K8055_ERROR = -1, K8055_ERROR_INIT_LIBUSB = -2, /* error during libusb initialization */
	K8055_ERROR_NO_DEVICES = -3, /* no usb devices found on host machine */
	K8055_ERROR_NO_K8055 = -4, /* Velleman k8055 cannot be found (on given port) */
	K8055_ERROR_ACCESS = -6, /* access denied (insufficient permissions) */
	K8055_ERROR_OPEN = -7, /* error opening device handle (also applies for claiming and detaching kernel driver) */
	K8055_ERROR_CLOSED = -8, /* device is already closed */
	K8055_ERROR_WRITE = -9, /* write error */
	K8055_ERROR_READ = -10, /* read error */
	K8055_ERROR_INDEX = -11 /* invalid argument (i.e. trying to access analog channel >= 2) */
};

/**Opens a K8055 device on the given port (i.e. address).
 * @return 0 on success
 * @return K8055_ERROR_INIT_LIBUSB on libusb initialization error
 * @return K8055_ERROR_NO_DEVICES if no usb devices are found on host system
 * @return K8055_ERROR_NO_K8055 if no K8055 board is found at the given port
 * @return K8055_ERROR_ACCESS if permission is denied to access a usb port
 * @return K8055_ERROR_OPEN if another error occured preventing the board to be opened */
int openDevice(int port);

/** Closes a board at the given port. */
void closeDevice(int port);

/**Sets all digital ouputs according to the given bitmask.
 * @param port address of board
 * @param bitmask '1' for 'on', '0' for 'off'
 * @return K8055_ERROR_INDEX if port is an invalid index
 * @return K8055_ERROR_CLOSED if the board associated to the given port is not open
 * @return K8055_ERROR_WRITE if another error occurred during the write process */
int setAllDigital(int port, int digitalBitMask);

/**Sets a digital output at given channel.
 * @param port address of board
 * @param channel channel of port
 * @param status output status: '1' for 'on', '0' for 'off'
 * @return K8055_ERROR_INDEX if port is an invalid index
 * @return K8055_ERROR_CLOSED if the board associated to the given port is not open
 * @return K8055_ERROR_WRITE if another error occurred during the write process */
int setDigital(int port, int channel, int status);

/**Sets the values of both analog outputs.
 * @param port address of board
 * @param analog1 value of first analog output
 * @param analog2 value of second analog output
 * @return K8055_ERROR_INDEX if port is an invalid index
 * @return K8055_ERROR_CLOSED if the board associated to the given port is not open
 * @return K8055_ERROR_WRITE if another error occurred during the write process */
int setAllAnalog(int port, int analog1, int analog2);

/**Sets the value for an analog output at a given channel.
 * @param port address of board
 * @param channel channel of analog output (zero indexed)
 * @param value value of analog output [0-255]
 * @return K8055_ERROR_INDEX if port or channel are an invalid indices
 * @return K8055_ERROR_CLOSED if the board associated to the given port is not open
 * @return K8055_ERROR_WRITE if another error occurred during the write process */
int setAnalog(int port, int channel, int value);

/**Resets a hardware integrated counter of the Velleman K8055 board.
 * @param port address of board
 * @param counter index of counter (zero indexed)
 * @return K8055_ERROR_INDEX if port or counter are invalid indices
 * @return K8055_ERROR_CLOSED if the board associated to the given port is not open
 * @return K8055_ERROR_WRITE if another error occurred during the write process */
int resetCounter(int port, int counter);

/**Sets the debounce time of a hardware integrated counter of the Velleman K8055 board.
 * @param port address of board
 * @param counter index of counter (zero indexed)
 * @param debounce debounce value
 * @return K8055_ERROR_INDEX if port or counter are invalid indices
 * @return K8055_ERROR_CLOSED if the board associated to the given port is not open
 * @return K8055_ERROR_WRITE if another error occurred during the write process*/
int setDebounceTime(int port, int counter, int debounce);

/**Reads all current data of a given board into the passed parameters. NULL is a valid parameter.
 * @param port address of board
 * @param digitalBitmask bitmask value of digital inputs
 * @param analog1 value of first analog input
 * @param analog2 value of second analog input
 * @param counter1 value of first counter
 * @param counter2 value of second counter
 * @return 0 on success
 * @return K8055_ERROR_INDEX if port or counter are invalid indices
 * @return K8055_ERROR_CLOSED if the board associated to the given port is not open
 * @return K8055_ERROR_READ if another error occurred during the read process */
int getAll(int port, int *digitalBitMask, int *analog1, int *analog2, int *counter1, int *counter2);

#endif /* K8055_H_ */
