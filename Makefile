DESTDIR=
PREFIX=$(DESTDIR)/usr

compile:
	make -C src

copy: compile mkdirs
	cp src/*.so target/lib
	cp src/*.h target/include

clean:
	make clean -C src
	rm -rf target

dist-clean: clean

mkdirs:
	mkdir -p target/lib
	mkdir -p target/include

doc: mkdirs
	doxygen Doxyfile

#run this if you want to build everything but not install user-wide or system-wide
local: compile copy

#these commands must be run as root
install-rules:
	cp k8055.rules $(DESTDIR)/etc/udev/rules.d/k8055.rules

uninstall-rules:
	rm $(DESTDIR)/etc/udev/rules.d/k8055.rules

install-permissions: install-rules
	groupadd -f k8055
	$(foreach user, $(USERS), usermod -a -G k8055 $(user);)

uninstall-permissions: uninstall-rules
	groupdel k8055

install: compile
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp src/*.so $(PREFIX)/lib
	cp src/*.h $(PREFIX)/include

uninstall:
	rm $(PREFIX)/lib/libk8055.so
	rm $(PREFIX)/include/k8055.h
