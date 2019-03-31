all: compile

compile: fileCompressor.c compressor.c data.c huffman.c
	gcc -c compressor.c data.c huffman.c
	gcc fileCompressor.c compressor.o data.o huffman.o -o fileCompressor

clean:
	rm -rf compressor.o fileCompressor data.o huffman.o huffman

h: huffman.c
	gcc -c huffman.c
	gcc huffmanTest.c huffman.o -o huffman
