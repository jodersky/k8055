PREFIX = /usr/local

#run this if you want to build everything but not install user-wide or system-wide
local: compile copy

compile:
	make -C src

copy: compile mkdirs
	cp src/*.so target/lib
	cp src/*.h target/include

clean:
	make clean -C src
	rm -rf target

mkdirs:
	mkdir -p target
	mkdir -p target/lib
	mkdir -p target/include

doc: mkdirs
	doxygen Doxyfile

#these commands must be run as root
install-rules:
	cp k8055.rules /etc/udev/rules.d/k8055.rules

uninstall-rules:
	rm /etc/udev/rules.d/k8055.rules

install-permissions: install-rules
	groupadd -f k8055
	$(foreach user, $(USERS), usermod -a -G k8055 $(user);)

uninstall-permissions: uninstall-rules
	groupdel k8055

install-product: compile
	cp src/*.so $(PREFIX)/lib
	cp src/*.h $(PREFIX)/include

uninstall-product:
	rm $(PREFIX)/lib/libk8055.so
	rm $(PREFIX)/include/k8055.h
	
install: install-product install-permissions

uninstall: uninstall-permissions uninstall-product

