#include "compressor.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Error!\n");
		return 0;
	}
	int fd = open("test.test", O_WRONLY | O_CREAT);
	if (fd == -1) {
		printf("Error 2!\n");
		return 0;
	}
	test(argv[1], fd);
	//tokenize(argv[1]);
	return 0;
}
