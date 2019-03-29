all: compile

compile: fileCompressor.c compressor.c
	gcc -c compressor.c data.c heap.c
	gcc fileCompressor.c compressor.o -o data.o -o heap.o -o fileCompressor

clean:
	rm -rf compressor.o fileCompressor data.o heap.o
