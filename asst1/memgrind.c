#include "mymalloc.h"

int main(int argc, char** argv) {
	char* vp;
	vp = malloc(17);
	print();
	*(short*)(vp-2) = -17;
	malloc(-12);
	malloc(17);
	malloc(45);
	malloc(33);
	print();
	free(vp);
	return 0;
}