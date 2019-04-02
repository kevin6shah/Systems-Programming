all: compile

compile: fileCompressor.c compressor.c data.c
	gcc -c compressor.c data.c
	gcc fileCompressor.c compressor.o data.o -o fileCompressor

clean:
	rm -rf compressor.o fileCompressor data.o test test.tx HuffmanCodebook test.txt.hcz

t:
	./fileCompressor -b test.txt ./
	./fileCompressor -c test.txt ./HuffmanCodebook	
	gcc -c data.c compressor.c
	gcc test.c data.o compressor.o -o test
	./test ./test.txt.hcz ./HuffmanCodebook
	cat HuffmanCodebook
	cat test.txt
	cat test.tx

H:
	mkdir hello
	cp test.txt hello
	gcc -c compressor.c data.c
	gcc fileCompressor.c compressor.o data.o -o fileCompressor
	./fileCompressor -b -R hello
	./fileCompressor -c -R hello ./HuffmanCodebook
	cat hello/test.txt
	rm hello/test.txt
	./fileCompressor -d -R hello ./HuffmanCodebook
	hello/test.txt > ../modified.txt
	cat hello/test.txt
	rm -rf hello
	make clean