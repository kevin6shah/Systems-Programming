#include "mymalloc.h"
#include <time.h>

int testA();
int testB();
int testC();
int testD();
int testE();
int testF();

int main(int argc, char** argv) {
	double timeTaken;
	double A = 0.0, B = 0.0, C = 0.0, D = 0.0, E = 0.0, F = 0.0;
	int i;
	clock_t start, end;
	for (i = 0; i < 100; i++) {
		start = clock();
		testA();
   		end = clock();
    	timeTaken = (double)(end-start)/CLOCKS_PER_SEC;
    	A+=timeTaken;
    	start = clock();
		testB();
   		end = clock();
    	timeTaken = (double)(end-start)/CLOCKS_PER_SEC;
    	B+=timeTaken;
    	start = clock();
		testC();
   		end = clock();
    	timeTaken = (double)(end-start)/CLOCKS_PER_SEC;
    	C+=timeTaken;
    	start = clock();
		testD();
   		end = clock();
    	timeTaken = (double)(end-start)/CLOCKS_PER_SEC;
    	D+=timeTaken;
    	start = clock();
		testE();
   		end = clock();
    	timeTaken = (double)(end-start)/CLOCKS_PER_SEC;
    	E+=timeTaken;
    	start = clock();
		testF();
   		end = clock();
    	timeTaken = (double)(end-start)/CLOCKS_PER_SEC;
    	F+=timeTaken;
    }
    A/=100;
    printf("Average Time Taken by Test Case A: %f seconds\n", A);
    B/=100;
    printf("Average Time Taken by Test Case B: %f seconds\n", B);
    C/=100;
    printf("Average Time Taken by Test Case C: %f seconds\n", C);
    D/=100;
    printf("Average Time Taken by Test Case D: %f seconds\n", D);
    E/=100;
    printf("Average Time Taken by Test Case E: %f seconds\n", E);
    F/=100;
    printf("Average Time Taken by Test Case F: %f seconds\n", F);
    return 0;
}


int testA() {
	int i;
	for (i = 0; i < 150; i++) {
		char* ptr = (char*) malloc(1);
		if (ptr == NULL) return 0;
		free(ptr);
	}
	return 1;
}
 



int testB() {
	char *ptr[150];
	int i, j, condition;
	for (i = 0; i < 150; i++) {
		ptr[i] = (char*) malloc(1);
		if (ptr[i] == NULL) return 0;
		if (i == 49 || i == 99 || i == 149) {
			if (i == 49) {
				j = 0;
				condition = 50;
			} else if (i == 99) {
				j = 50;
				condition = 100;
			} else {
				j = 100;
				condition = 150;
			}
			for (; j < condition; j++) {
				free(ptr[j]);
			}
		}
	}
	return 1;
}


int testC(){
    int operation = 0; //if operation = 1 malloc, if operation = 0 free
    int malloc_counter = 0;
    int free_counter = 0;
    void *pointer_holder[50];
    int index = 0;
    srand(time(NULL));
    while(1){
        if (malloc_counter == 50) break;
        operation = rand() % 2;
        if (operation == 1){
            void *ptr = malloc(1);
            malloc_counter++;
            pointer_holder[index] =  ptr;
            index++;
            continue;
        }
        if (operation == 0 && (malloc_counter - free_counter) > 0){
            free(pointer_holder[free_counter]);
            free_counter++;
        }
        
    }
    while ((malloc_counter - free_counter) > 0){
        free(pointer_holder[free_counter]);
        free_counter++;
    }
    return 1;
}

int testD(){
    int operation = 0;
    int malloc_counter = 0;
    int free_counter = 0;
    void *pointer_holder [50];
    //int total_malloced = 0;
    int index = 0;
    srand(time(NULL));
    while(1){
        if (malloc_counter == 50) break;
        operation = rand() % 2;
        if (operation == 1){
            int malloc_size = rand() % 64;
            malloc_size++;
            void *ptr = malloc(malloc_size);
            //total_malloced += (malloc_size + 2);
            pointer_holder[index] = ptr;
            index++;
            malloc_counter++;
            continue;
        }
        if (operation == 0 && (malloc_counter - free_counter) > 0){
            free(pointer_holder[free_counter]);
            free_counter++;
        }
    
    }
    while ((malloc_counter - free_counter) > 0){
        free(pointer_holder[free_counter]);
        free_counter++;
    }
    //printf("total malloced = %d\n", total_malloced);
    return 1;

}

//will allocate array to maximum capacity in mallocs of 1 byte
//1364 mallocs will fill myblock 4092/4096 bytes
//due to fragmentation, it will actually fill it 4094/4096 bytes
//the two missing bytes are the magic number
//then free each byte randomly
int testE(){
    int malloc_counter = 0;
    int index = 0;
    void *pointer_holder[1364];
    srand(time(NULL));
    while(malloc_counter < 1364){
        pointer_holder[index] = malloc(1);
        malloc_counter++;
        index++;
    }
    int free_counter = 0;
    while (free_counter < 1364){
        int i = rand() % 1364;
        if (pointer_holder[i] != NULL){
            free(pointer_holder[i]);
            //print();
            free_counter++;
            pointer_holder[i] = NULL;
        }
    }
    return 1;
    
    
    
    
}


//will completely allocate the myblock in chunks of 100
//make 40 blocks
/*then free random blocks
 *then reallocate memory in size of 33
 *do this 150 times
 *then free the rest of the space
 */
int testF(){
    srand(time(NULL));
    int malloc_counter = 0;
    int index = 0;
    void *pointer_holder[200];
    while (malloc_counter < 40){
        pointer_holder[index] = malloc(100);
        malloc_counter++;
        index++;
    }
    
    //index++;
    malloc_counter--;
    int i = 0;
    while(i < 150){
        int ind = rand() % malloc_counter;
        if (pointer_holder[ind] == NULL) continue;
        free(pointer_holder[ind]);
        pointer_holder[ind] = NULL;
        pointer_holder[index] = malloc(33);
        index++;
        malloc_counter++;
        i++;
        }
    int j;
    for(j = 0; j < malloc_counter+1; j++){
        if (pointer_holder[j] == NULL) continue;
        free(pointer_holder[j]);
    }
    return 1;
             
}

