#include "compressor.h"
#include "data.h"

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

int bufferSize(char* str) {
	int fd = open(str, O_RDONLY);
	if (fd <= 0) {
		printf("Could not open the file: %s\n", str);
		return -1;
	}
	char temp;	// Try taking this temp out
	int counter = 0;
	while (read(fd, &temp, 1) != 0) {
		counter++;
	}
	close(fd);
	return counter;
}

char* findBuffer(char* str) {
	int bufSize = bufferSize(str);
	if (bufSize == -1) return NULL;
	int fd = open(str, O_RDONLY);
	char *buffer = malloc(++bufSize);
	int result = read(fd, buffer, bufSize);
	printf("%d\n", result);
	if (result < 0) {
		printf("Could not read the file: %s\n", str);
		return NULL;
	}
	buffer[result] = '\0';
  return buffer;
}
