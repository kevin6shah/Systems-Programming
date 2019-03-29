all: compile

compile: fileCompressor.c compressor.c
	gcc -c compressor.c data.c
	gcc fileCompressor.c compressor.o -o data.o -o fileCompressor

clean:
	rm -rf compressor.o fileCompressor data.o
