#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

	char *path = "testing";
	char *rrename = "testing_1";
	rename(path, rrename);
	return 0;


}
