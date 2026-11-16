all: build output.mp4

output.mp4: build/out
	./build/out | ffmpeg -y -f image2pipe -vcodec ppm -framerate 120 -i pipe:0 -crf 12 -preset veryslow -c:v libx264 -pix_fmt yuv444p output.mp4

build/out: demo.c build/utah_teapot.c
	gcc -Wall -Wextra -Wconversion -O3 -march=native -mavx2 -lm $< -o $@

build/utah_teapot.c: ../../../assets/utah_teapot.obj build
	../../../assets/obj2c.py $< $@ utah_teapot

build:
	mkdir -p build

play: output.mp4
	mpv --loop=inf output.mp4

clean:
	rm -rf output.mp4 build
