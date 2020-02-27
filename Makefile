all: main

main.o: main.c
	gcc -m64 -c main.c

mandelbrot_x86.o: mandelbrot_x86.s
	nasm -f elf64 mandelbrot_x86.s 

main: main.o mandelbrot_x86.o
	gcc -m64 -no-pie main.o mandelbrot_x86.o -o main -I/usr/include/x86_64-linux-gnu -lallegro_font -lallegro

clean:
	rm *.o
	rm main
