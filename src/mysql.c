#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

static char *host;
static char *user;
static char *pass;
static char *db;

void set_db_state(char *h, char *u, char *p, char *d) {
	host = h;
	user = u;
	pass = p;
	db = d;
}

void bmig_init(MYSQL *connection) {
	char *db =
		"create table if not exists zzzzzbmigmigrations (name varchar(255) not null, \
		primary key(name))engine=innodb default charset=utf8;";
	mysql_query(connection, db);
}

MYSQL *get_mysql_conn() {
	MYSQL *connection = mysql_init(NULL);

	if (connection == NULL) {
		printf("cannot get a connection\n\n");
		return NULL;
	}

	if (mysql_real_connect(
		connection,
		host, user, pass, db,
		0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {

		printf("error connecting to server\n\n");
		exit(1);
	}

	bmig_init(connection);

	return connection;
}

void get_remote_status(MYSQL *connection, char *local_mig, int *remote_mig) {
	mysql_query(connection, "select * from zzzzzbmigmigrations");

	MYSQL_RES *result = mysql_store_result(connection);
	MYSQL_ROW row;

	while (row = mysql_fetch_row(result)) {
		int pos = in_array((char *)row[0], local_mig, sizeof(remote_mig));

		if (pos > -1) {
			remote_mig[pos] = 1;
		}
	}

	mysql_free_result(result);
}
