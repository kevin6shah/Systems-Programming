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
