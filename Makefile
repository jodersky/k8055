PREFIX=/usr/local
UDEV=/etc/udev/rules.d

build:
	make -C src libk8055.so

clean:
	make clean -C src
	rm -rf target

doc:
	mkdir -p target
	doxygen Doxyfile

#run this if you want to build everything but not install user-wide or system-wide
local: build
	mkdir -p target/lib
	mkdir -p target/include
	cp -P src/*.so* target/lib
	cp src/*.h target/include

#these commands must be run as root
install-rules:
	cp k8055.rules $(UDEV)/k8055.rules

uninstall-rules:
	rm -f $(UDEV)/k8055.rules

install-permissions: install-rules
	groupadd --force --system k8055
	$(foreach user, $(USERS), usermod -a -G k8055 $(user);)

uninstall-permissions: uninstall-rules
	groupdel k8055

install: build
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp -P src/*.so* $(PREFIX)/lib
	cp src/*.h $(PREFIX)/include

uninstall:
	rm $(PREFIX)/lib/libk8055.so*
	rm $(PREFIX)/include/k8055.h
