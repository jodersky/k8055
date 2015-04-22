# k8055 for libusb

This library provides access to the Velleman K8055 USB Experiment Board. While providing a high-level interface to the user, the library communicates with a k8055 board through [libusb](http://www.libusb.org/wiki/libusb-1.0).

## Main Features
- runs with libusb-1.0
- up to 4 k8055 boards supported simultaneously (limit is given by k8055 hardware)
- pseudo-querying of a board's output status (see header file documentation for detailed explanation)
- concise and lightweight

## Requirements
- libusb-1.0
- (doxygen for documentation generation)

## Build
### Local build
Run `make local` in the project root folder. Products are copied to 'target' directory.

To remove all generated files, run `make clean`.

### System install
Run  `make install` to install the library and header files (this command does essentially the same as a local build with the exception that products are copied to /usr/local/ by default). You may change that path by passing 'make' the variable 'PREFIX', i.e. `make install PREFIX=/my/custom/path`. To uninstall, run `make uninstall`.

### Udev Rules
If your system uses udev (i.e. linux), you will probably have to configure it to allow access to the k8055 boards. The following instructions show how to configure udev.

1. Copy the file `k8055.rules' into the udev rules directory, typically /etc/udev/rules.d

    `cp k8055.rules /etc/udev/rules.d/k8055.rules`

2. Create a group named k8055.

    `groupadd -r k8055`

3. Add to the group yourself and/or other users whom you wish to grant access to the boards.

    `usermod -a -G k8055 $(USER)`

The previously described steps may be automated by running `make install-rules` to install just the rules or `make install-permissions USERS="<list of users>"` to also create the group and add the given list of users to it.
To uninstall, run `make uninstall-rules` or `make uninstall-permissions`.

## Documentation
See the comments in the source code for documentation about usage. Note: the API isn't the same as the one provided by Velleman in their DLL.

Run `make doc` to generate doxygen documentation.

## Authors
The library is written by Jakob Odersky. It is based on the information provided by the following people who wrote the original version of `libk8055' (http://libk8055.sourceforge.net) :

- 2005 by Sven Lindberg <k8055@k8055.mine.nu>

- 2007 by Pjetur G. Hjaltason <pjetur@pjetur.net> Commenting, general rearrangement of code, bugfixes, python interface with swig and simple k8055 python class


## Copying
Copyright (c) 2012 by Jakob Odersky

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

Prior to version 1.0

- Ubuntu 11.04, kernel 2.6.38-13-generic, x86_64, March 2012
- Ubuntu 11.10, kernel 3.0.0-16-generic, i686, March 2012
- Ubuntu 12.04, kernel 3.2.0-24-generic, x86_64, May 2012

Version 1.0

- Ubuntu 12.04, kernel 3.2.0-26-generic, x86_64, July 2012
