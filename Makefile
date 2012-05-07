
all: compile copy

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
	$(foreach user, $(users), usermod -a -G k8055 $(user);)
uninstall-permissions: uninstall-rules
	groupdel k8055


