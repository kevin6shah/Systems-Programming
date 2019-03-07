#include "mymalloc.h"

int main(int argc, char** argv) {
	char* vp;
	vp = malloc(17);
	malloc(-12);
	vp = malloc(17);
	malloc(45);
	malloc(33);
	print();
	void *a = malloc(9);
    free(a);
    free(a);
    free(vp);
    void *p = malloc(123);
    //malloc(455);
    free(p);
    
	print();
	return 0;
}
