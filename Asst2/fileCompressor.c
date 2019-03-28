#include "compressor.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Error!\n");
		return 0;
	}
	test(argv[1]);
	return 0;
}
