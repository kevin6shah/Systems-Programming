#ifndef FILE_COMPRESSOR
#define FILE_COMPRESSOR

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "data.h"

void test(char* str, int fd);
char* tokenize(char* str);

#endif
