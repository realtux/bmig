#include <stdio.h>
#include <string.h>
#include <mysql.h>

void bmig_init(MYSQL *connection) {
	char *db =
		"create table if not exists zzzzzbmigmigrations (name varchar(255) not null, \
		primary key(name))engine=innodb default charset=utf8;";
	mysql_query(connection, db);
}

MYSQL *get_mysql_conn() {
	MYSQL *connection = mysql_init(NULL);

	if (connection == NULL) {
		printf("cannot get a connection");
		return NULL;
	}

	mysql_real_connect(
		connection,
		"localhost", "root", "root", "apiowl",
		0, NULL, CLIENT_MULTI_STATEMENTS);

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
