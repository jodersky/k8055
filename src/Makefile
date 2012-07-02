C = gcc
CFLAGS = -std=c99 -O2 -Wall -pedantic
VERSION_MAJOR=1
VERSION_MINOR=0
VERSION=$(VERSION_MAJOR).$(VERSION_MINOR)

libk8055.so: libk8055.so.$(VERSION)
	ln -s libk8055.so.$(VERSION) libk8055.so

libk8055.so.$(VERSION): k8055.o
	$(C) $(CFLAGS) -shared -Wl,-soname,libk8055.so.$(VERSION_MAJOR) -o libk8055.so.$(VERSION) k8055.o -lusb-1.0 -lm

k8055.o: k8055.c
	$(C) $(CFLAGS) -fPIC -c k8055.c -o k8055.o

clean:
	rm -rf *.o
	rm -rf *.so*
	rm -rf k8055-*

# test and benchmark programs
test: k8055.c test.c
	$(C) test.c k8055.c -o k8055-test $(CFLAGS) -D_POSIX_C_SOURCE=199309L -lusb-1.0 -lm

benchmark: k8055.c benchmark.c
	$(C) benchmark.c k8055.c -o k8055-benchmark $(CFLAGS) -D_POSIX_C_SOURCE=199309L -lusb-1.0 -lm
