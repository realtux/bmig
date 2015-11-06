#ifndef UTIL_H
#define UTIL_H

#define infinite for(;;)

int array_size(const char **);
int in_array(const char *, const char **);
int cstring_cmp(const void *, const void *);
void get_timestamp(char *);
char *read_file(const char *, long *);
int make_migrations_dir(void);

#endif
