#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int in_array(char *value, char **array, size_t size) {
	size_t i;

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

char *read_file(char *filename) {
	FILE *f = fopen(filename, "rb");

	if (f == NULL) {
		printf("error opening migration file");
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *string = malloc(fsize + 1);
	fread(string, fsize, 1, f);

	fclose(f);

	string[fsize] = 0;
	
	return string;
}
