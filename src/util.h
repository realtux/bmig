#ifndef UTIL_H
#define UTIL_H

#define infinite for(;;)

int in_array(const char *, const char **, size_t );
int cstring_cmp(const void *, const void *);
void get_timestamp(char *);
char *read_file(const char *, long *);

#endif
