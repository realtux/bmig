#ifndef MYSQL_H
#define MYSQL_H

void set_db_state(const char *, const char *, const char *, const char *);
void bmig_init(MYSQL *);
MYSQL *get_mysql_conn(void);
void get_remote_status(MYSQL *, const char **, int *);
void run_migs(const char *, long);

#endif
