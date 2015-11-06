/***
The MIT License (MIT)

Copyright (c) 2015 Brian Seymour

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
***/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

int array_size(const char **array) {
	int i = 0;

	while (array[i] != NULL) ++i;

	return i;
}

int in_array(const char *value, const char **array) {
	int size = array_size(array);

	int i;
	for (i = 0; i < size; ++i) {
		if (array[i] == NULL) return -1;

		if (strcmp(array[i], value) == 0) {
			return i;
		}
	}

	return -1;
}

int cstring_cmp(const void *a, const void *b) {
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;

    return strcmp(*ia, *ib);
}

void get_timestamp(char *buf) {
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buf, 16, "%G%m%d%H%M%S", timeinfo);
}

char *read_file(const char *filename, long *size) {
	FILE *f = fopen(filename, "rb");

	if (f == NULL) {
		printf("error opening file\n\n");
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	// hand back size of file
	*size = fsize;

	char *string = malloc(fsize + 1);

	if (string == NULL) {
		printf("memory allocation error\n\n");
		exit(1);
	}

	fread(string, fsize, 1, f);

	fclose(f);

	string[fsize] = 0;

	return string;
}

void make_migrations_dir(void) {
	if (mkdir("migrations/", EEXIST) == -1) {
		printf("migrations folder exists already, cannot create\n");
	} else {
		printf("migrations folder created\n");
	}
}
