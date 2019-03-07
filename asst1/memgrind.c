#include "mymalloc.h"

int main(int argc, char** argv) {
	/*char* vp;
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
    free(p);*/
    
	//print();
	//*(short*)(vp-2) = -17;
	//malloc(-12);
	void *a = malloc(18);
	void *b = malloc(45);
	void *c = malloc(33);
    void *d = malloc(20);
	//void *b = malloc(3980);
	//void *a = malloc(9);
    free(a);
    free(c);
    free(b);
    //free(vp);
    //free(b);
    //free(b);
	print();
	return 0;
}
