# k8055 for libusb-1.0

This library provides access to the Velleman K8055 USB Experiment Board for operating systems supporting libusb-1.0 (http://www.libusb.org/wiki/libusb-1.0), such as Linux. While providing a high-level interface to the user, the library communicates with a k8055 board through libusb.

## Main Features
- runs with libusb-1.0
- up to 4 k8055 boards supported simultaneously (limit is given by k8055 hardware)
- pseudo-querying of a board's ouput status (see header file documentation for detailed explanation)
- lightweight (one source file, one header file => one shared library)

## Requirements
- libusb-1.0 (see src/Makefile to set include path)
- (doxygen for documentation generation)

## Build
Run ```make``` in the project root folder. Products are copied to `target' directory.

### Udev Rules
If your system uses udev, you will probably have to configure it to allow access the k8055 boards. The following instructions show how to configure udev (you will need root privileges).

1. Copy the file `k8055.rules' into the udev rules directory, typically /etc/udev/rules.d

    ```cp k8055.rules /etc/udev/rules.d/k8055.rules```

2. Create a group named k8055.

    ```groupadd k8055```

3. Add to the group yourself and/or other users whom you wish to grant access to the boards.

    ```usermod -a -G k8055 $(USER)```

The previously described steps may be automated by running ```make install-rules``` to install just the rules or ```make install-permissions users="<list of users>"``` to also create the group and add the given list of users to it.
To uninstall, run ```make uninstall-rules``` or ```make uninstall-permissions```.


## Documentation
See the comments in the source code for documentation about usage. Note: the API isn't the same as the one provided by Velleman in their DLL.

Run ```make doc``` to generate doxygen documentation.

## Authors
The library is written by Jakob Odersky. It is based on the information provided by the following people who wrote the original version of `libk8055' (http://libk8055.sourceforge.net) :

- 2005 by Sven Lindberg <k8055@k8055.mine.nu>

- 2007 by Pjetur G. Hjaltason <pjetur@pjetur.net> Commenting, general rearrangement of code, bugfixes, python interface with swig and simple k8055 python class


## License
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

## Tested
- Ubuntu 11.04, kernel 2.6.38-13-generic, x86_64, March 2012
- Ubuntu 11.10, kernel 3.0.0-16-generic, i686, March 2012
- Ubuntu 12.04, kernel 3.2.0-24-generic, x86_64, May 2012
