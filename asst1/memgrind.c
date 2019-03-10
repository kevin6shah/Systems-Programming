#include "mymalloc.h"
#include <time.h>

int testA();
int testB();
int testC();
int testD();
int testE();
int testF();

int main(int argc, char** argv) {
	if (testA()) {
		printf("TestA Success!\n");
	}
	if (testB()) {
		printf("TestB Success!\n");
	}
    if (testC()) {
        printf("TestC Success!\n");
    }
    if (testD()) {
        printf("TestD Success!\n");
    }
    if (testE()) {
        printf("TestE Success!\n");
    }
    if (testF()) {
        printf("TestF Success!\n");
    }
    
}


int testA() {
	int i;
	for (i = 0; i < 150; i++) {
		char* ptr = (char*) malloc(1);
		if (ptr == NULL) return 0;
		free(ptr);
	}
	print();
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
	print();
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
    print();
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
    print();
    //printf("total malloced = %d\n", total_malloced);
    return 1;

}

//will allocate memory in random intervals
//till reaches maximum capacity
int testE(){
    
    
    
    
    
}



int testF(){
    
    
    
}

