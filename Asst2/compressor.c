#include "compressor.h"

void test(char* str) {
	DIR *directory = opendir(str);
	struct dirent* temp;
	if (directory == NULL) {
		printf("Could not find the directory!\n");
		return;
	}
	int i = 0;
	while ((temp = readdir(directory)) != NULL) {
		if (temp->d_type == 4 && strcmp(temp->d_name, ".") != 0 && strcmp(temp->d_name, "..") != 0) {
			char* buffer = malloc(2+temp->d_namlen+strlen(str));
			strcpy(buffer, str);
			strcat(buffer, "/");
			strcat(buffer, temp->d_name);
			buffer[strlen(buffer)] = '\0';
			test(buffer);
			free(buffer);
		}
		else {
			if (!(i < 2)) {
				printf("%s/%s\n", str, temp->d_name);
			}
		}
		i++;
	}
	closedir(directory);
}