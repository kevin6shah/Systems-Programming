#include "compressor.h"

void test(char* str, int fd) {
	DIR *directory = opendir(str);
	struct dirent* temp;
	if (directory == NULL) {
		printf("Could not find the directory!\n");
		return;
	}
	int i = 0;
	while ((temp = readdir(directory)) != NULL) {
		if (temp->d_type == 4 && strcmp(temp->d_name, ".") != 0 && strcmp(temp->d_name, "..") != 0) {
			char* buffer = malloc(2+temp->d_reclen+strlen(str));
			strcpy(buffer, str);
			strcat(buffer, "/");
			strcat(buffer, temp->d_name);
			buffer[strlen(buffer)] = '\0';
			test(buffer, fd);
			free(buffer);
		}
		else {
			if (!(i < 2)) {
				int count = 3+strlen(str)+temp->d_reclen;
				char* buffer = malloc(count);
				strcpy(buffer, str);
				strcat(buffer, "/");
				strcat(buffer, temp->d_name);
				buffer[strlen(buffer)] = '\0';
				buffer[strlen(buffer)] = '\n';
				write(fd, buffer, count);
			}
		}
		i++;
	}
	closedir(directory);
}

char* tokenize(char* str) {
	int fd = open(str, O_RDONLY);
	if (fd == -1) {
		printf("ERROR\n");
		return NULL;
	}
	char temp;
	int result, counter;
	while ((result = read(fd, &temp, 1)) != 0) {
		counter++;
	}
	close(fd);
	fd = open(str, O_RDONLY);
	char *buffer = malloc(++counter);
	result = read(fd, buffer, counter);
	if (result == -1) {
		printf("ERROR 2\n");
		return NULL;
	}
	buffer[result-1] = '\0';
    return buffer;
    
}



/*
char* tokenize(char *str){
    char * buffer = 0;
    int length;
    FILE *fd = fopen(str, "rb");
    if (fd){
        fseek(fd,0, SEEK_END);
        length = ftell (fd);
        fseek(fd, 0, SEEK_SET);
        buffer = malloc (length + 1);
        if (buffer){
            fread(buffer, 1, length, fd);
        }
        fclose(fd);
    }
    buffer[length] = '\0';
    return buffer;
}
 
 */
