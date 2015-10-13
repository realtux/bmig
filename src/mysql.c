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
