
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

doc:
	doxygen Doxyfile
