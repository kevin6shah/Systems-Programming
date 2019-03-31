all: compile

compile: fileCompressor.c compressor.c data.c heap.c
	gcc -c compressor.c data.c heap.c
	gcc fileCompressor.c compressor.o -o fileCompressor.c data.o -o fileCompressor.c heap.o -o fileCompressor

clean:
	rm -rf compressor.o fileCompressor data.o huffman.o huffman

h: huffman.c
	gcc -c huffman.c
	gcc huffmanTest.c huffman.o -o huffman
