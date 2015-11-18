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

#include "util.h"

static const char *host;
static const char *user;
static const char *pass;
static const char *db;

extern int flag_transaction;
extern int flag_bail;

void set_db_state(const char *h, const char *u, const char *p, const char *d) {
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

MYSQL *get_mysql_conn(void) {
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

void get_remote_status(MYSQL *connection, const char **local_mig, int local_mig_count, int **remote_mig) {
	mysql_query(connection, "select * from zzzzzbmigmigrations");

	MYSQL_RES *result = mysql_store_result(connection);
	MYSQL_ROW row;

	// allocate memory to fit total records in the db migrations, zero init
	*remote_mig = calloc(local_mig_count, sizeof(int) * local_mig_count);

	if (*remote_mig == NULL) {
		printf("memory allocation error\n\n");
		exit(1);
	}

	// check if remote mig exists in local folder, flag as 1 if so
	while ((row = mysql_fetch_row(result))) {
		int pos = in_array((char *)row[0], local_mig);

		if (pos > -1) (*remote_mig)[pos] = 1;
	}

	mysql_free_result(result);
}

void run_migs(const char *mig, long mig_size) {
	MYSQL *connection;

	char *cur_command = malloc(mig_size + 1);

	if (cur_command == NULL) {
		printf("memory allocation error\n\n");
		exit(1);
	}

	int c;
	int x = 0;
	int mig_len = strlen(mig);

	for (c = 0; c < mig_len; ++c) {
		// continue copying command
		cur_command[x] = mig[c];

		// check if it's the end of the line, or end of the mig
		if ((mig[c] == ';' && mig[c + 1] == '\n') ||
			(mig[c] == ';' && c == mig_len - 1)) {

			// terminate the command for the connection
			cur_command[x + 1] = '\0';

			// run the up
			connection = get_mysql_conn();
			if (mysql_query(connection, cur_command)) {
				// error
				printf("\033[0;31merror: \033[0m%s\n", mysql_error(connection));
			}
			mysql_close(connection);

			// reset where to copy pointer to
			x = 0;

			// push past newline
			++c;
			continue;
		}

		++x;
	}

	free(cur_command);
}
