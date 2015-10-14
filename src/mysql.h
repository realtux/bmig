#ifndef MYSQL_H
#define MYSQL_H

void set_db_state(char *h, char *u, char *p, char *d);
void bmig_init(MYSQL *connection);
MYSQL *get_mysql_conn();
void get_remote_status(MYSQL *connection, char **local_mig, int *remote_mig);

#endif
