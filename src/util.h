#ifndef UTIL_H
#define UTIL_H

int in_array(char *value, char **array, size_t size);
int cstring_cmp(const void *a, const void *b);
void get_timestamp(char *buf);
char *read_file(char *filename, long *mig_size);

#endif
