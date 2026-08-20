all : lib
	mkdir -p build
	g++ src/main.cpp -o build/main -Wall -Wextra -Iinclude -Llib -lraylib -lX11

lib :
	wget -q -O raylib-6.0.tar.gz https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz
	tar -xzf raylib-6.0.tar.gz
	rm raylib-6.0.tar.gz

	cd raylib-6.0/src && make -j

	mkdir -p include
	cp raylib-6.0/src/raylib.h include/raylib.h
	mkdir -p lib
	cp raylib-6.0/src/libraylib.a lib/libraylib.a

	rm -r raylib-6.0

clear :
	rm -rf ./guix_sandbox
	rm -rf ./build
	rm -rf ./include
	rm -rf ./lib