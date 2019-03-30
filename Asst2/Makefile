all: compile

compile: fileCompressor.c compressor.c data.c heap.c
	gcc -c compressor.c data.c heap.c
	gcc fileCompressor.c compressor.o -o fileCompressor.c data.o -o fileCompressor.c heap.o -o fileCompressor

clean:
	rm -rf compressor.o fileCompressor data.o huffman heap.o

h: huffman.c
	gcc -c heap.c
	gcc huffman.c heap.o -o huffman
