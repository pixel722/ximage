CC = clang
CFLAGS = -Wall -Wextra -Wconversion -pedantic -std=c11 -O3 -ffreestanding -fno-builtin -flto --target=wasm32-unknown-unknown -c

all: build build/index.html

build/index.html: index.html build/ximage.b64
	cp -f index.html $@
	sed -i "s|WASM_BLOB|$$(cat build/ximage.b64)|" $@

build/ximage.b64: build/ximage.wasm
	base64 -w 0 build/ximage.wasm > build/ximage.b64

build/ximage.wasm: build/engine.o build/dvd.o build/points3d.o build/teapot.o
	wasm-ld --no-entry --export=draw_demo_a --export=draw_demo_b --export=draw_demo_c --export=canvas --export-memory $^ -o $@

build/engine.o: engine.c ../ximage.h engine.h
	$(CC) $(CFLAGS) $< -o $@

build/dvd.o: demos/dvd.c ../ximage.h engine.h
	$(CC) $(CFLAGS) $< -o $@

build/points3d.o: demos/points3d.c ../ximage.h engine.h
	$(CC) $(CFLAGS) $< -o $@

build/teapot.o: demos/teapot.c build/utah_teapot.c ../ximage.h engine.h
	$(CC) $(CFLAGS) $< -o $@

build/utah_teapot.c: ../assets/utah_teapot.obj
	../assets/obj2c.py $< $@ utah_teapot

deploy:
	cp build/index.html ../

build:
	mkdir -p build

clean:
	rm -rf build
