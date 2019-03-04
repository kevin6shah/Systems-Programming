#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int main(int argc, char** argv) {
	malloc(2056);
	free(24);
	return 0;
}