all: compile

compile: fileCompressor.c compressor.c
	gcc -c compressor.c
	gcc fileCompressor.c compressor.o -o fileCompressor

clean:
	rm -rf compressor.o fileCompressor