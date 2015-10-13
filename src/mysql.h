#ifndef MYSQL_H
#define MYSQL_H

MYSQL *get_mysql_conn();
void bmig_init(MYSQL *connection);
char *get_remote_migrations();

#endif
