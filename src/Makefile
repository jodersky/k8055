C = gcc
# libusb.h location
INCDIR = /usr/include/libusb-1.0 
CFLAGS = -std=c99 -O2 -Wall

all: libk8055.so
	
libk8055.so: k8055.o
	$(C) -shared -o libk8055.so k8055.o -lusb-1.0

k8055.o: k8055.c
	$(C) $(CFLAGS) -fPIC -I $(INCDIR) -c k8055.c -o k8055.o

clean:
	rm -rf *.o
	rm -rf *.so
