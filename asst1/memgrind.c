#include "mymalloc.h"

int main(int argc, char** argv) {
	char* vp;
	vp = malloc(12);
	vp = "hello my ho\0";
	printf("%s\n", vp);
	malloc(17);
	malloc(45);
	malloc(33);
	free(vp);
	return 0;
}