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
#include <dirent.h>
#include <mysql.h>
#include <string.h>
#include <ctype.h>
#include <json.h>

#include "util.h"
#include "mysql.h"
#include "config.h"

#define VERSION "0.2.0"
#define DEFAULT_MIGRATION_PATH "migrations/"

static char *migration_path;

int flag_transaction = 0;
int flag_bail = 0;

void menu(void) {
	printf("usage: bmig command\n");
	printf("\n");
	printf("    status\n");
	printf("        see the status of all migrations\n");
	printf("\n");
	printf("    create [name]\n");
	printf("        create a new migration\n");
	printf("\n");
	printf("    migrate\n");
	printf("        run all available migrations\n");
	printf("\n");
	printf("        -t    wrap each .sql in a transaction, use with -b\n");
	printf("              to avoid an out of sequence migration\n");
	printf("\n");
	printf("        -b    upon migration failure, don't proceed, use\n");
	printf("              with -t to avoid a half completed migration\n");
	printf("\n");
	printf("    rollback\n");
	printf("        rollback the last migration\n");
	printf("\n");
}

void populate_local_mig(char **local_mig) {
	// open parent dir
	DIR *dir;
	struct dirent *directory;

	dir = opendir(migration_path);

	if (dir == NULL) {
		printf("migrations folder not found\n\n");
		exit(1);
	}

	size_t i = 0;

	// construct local_mig full of .sql files
	while ((directory = readdir(dir)) != NULL) {
		int d_name_len = strlen(directory->d_name);

		char *file_name = malloc(sizeof(char *) * d_name_len + 1);

		strcpy(file_name, (const char *)directory->d_name);

		size_t len = strlen(file_name);

		// add only .sql files to the local_mig
		if (len > 4 && strcmp(file_name + len - 4, ".sql") == 0) {
			local_mig[i] = file_name;
			++i;
		}
	}

	qsort(local_mig, i, sizeof(char *), cstring_cmp);

	local_mig[i + 1] = '\0';

	closedir(dir);
}

void populate_up_down(char *mig, char *up, char *down) {
	int up_start = -1;
	int up_end = -1;
	int down_start = -1;
	int down_end = -1;

	// get up start
	char *result = strstr(mig, "up:");
	int pos = result - mig;

	up_start = pos >= 0 ? pos + 4 : -1;

	// get down start
	result = strstr(mig, "down:");
	pos = result - mig;

	down_start = pos >= 0 ? pos + 6 : -1;

	int mig_length = strlen(mig);

	// determine up end based on whether down is present
	up_end = down_start >= 1 ? down_start - 7 : mig_length;

	// determine down end based on whether up is present
	down_end = down_start >= 0 ? mig_length : -1;

	// load bodies
	int x = 0;
	int y = 0;

	if (up_start >= 0) {
		x = 0;
		for (y = up_start; y <= up_end; ++y) {
			up[x] = mig[y];
			++x;
		}
		up[x] = '\0';
	}

	if (down_start >= 0) {
		x = 0;
		for (y = down_start; y <= down_end; ++y) {
			down[x] = mig[y];
			++x;
		}
		down[x] = '\0';
	}
}

void parse_flags(int argc, const char **argv) {
	// check each flag
	int i;
	for (i = 2; i < argc; ++i) {
		// flags must begin with -
		if (*argv[i] != '-') continue;

		// check each character of each flag for potential matches
		int flag_len = strlen(argv[i]);

		int x;
		for (x = 1; x < flag_len; ++x) {
			switch (argv[i][x]) {
				case 't':
					flag_transaction = 1;
					break;
				case 'b':
					flag_bail = 1;
					break;
			}
		}
	}
}

int main(int argc, char **argv) {
	printf("bmig version %s\n", VERSION);

	// no command, needs a menu
	if (argc < 2) {
		menu();
		return 0;
	}

	parse_flags(argc, (const char **)argv);

	// read config file
	char *config = read_config();

	char *host = get_value(config, "host");
	char *user = get_value(config, "user");
	char *pass = get_value(config, "pass");
	char *db = get_value(config, "db");

	if (strlen(host) < 1) {
		printf("host not found in config file\n");
		return 1;
	}

	if (strlen(user) < 1) {
		printf("user not found in config file\n");
		return 1;
	}

	if (strlen(pass) < 1) {
		printf("pass not found in config file\n");
		return 1;
	}

	if (strlen(db) < 1) {
		printf("db not found in config file\n");
		return 1;
	}

	char *migs = get_value(config, "migs");

	migration_path = strlen(migs) > 0 ? migs : DEFAULT_MIGRATION_PATH;

	set_db_state(host, user, pass, db);

	char *command = argv[1];

	MYSQL *connection;

	connection = get_mysql_conn();

	size_t i = 0;

	// store parallel arrays for local/remote comparison
	char *local_mig[10000];
	int remote_mig[10000];

	// populate local_mig from fs
	populate_local_mig(local_mig);

	// populate remote_mig with 0/1 flags on local -> remote
	get_remote_status(connection, (const char **)local_mig, remote_mig);

	mysql_close(connection);

	// determine command
	if (strcmp(command, "status") == 0) {
		// check local_mig against remote_mig for differences
		i = 0;
		infinite {
			if (local_mig[i] == NULL) break;

			if (remote_mig[i] == 1) {
				printf("\033[0;32mup - \033[0m");
			} else {
				printf("\033[0;31mdn - \033[0m");
			}

			printf("%s\n", local_mig[i]);

			++i;
		}
	}

	if (strcmp(command, "create") == 0) {
		char *name = argc == 3 ? argv[2] : "migration";

		// make lowercase
		size_t i = 0;

		while (name[i]) {
			name[i] = tolower(name[i]);
			++i;
		}

		// get a timestamp
		char timestamp[16];
		get_timestamp(timestamp);

		char full_name[1024] = "";
		strcat(full_name, migration_path);

		// concat all the parts of the full mig name
		strcat(full_name, timestamp);
		strcat(full_name, "-");
		strcat(full_name, name);
		strcat(full_name, ".sql");

		// create file default
		char *template = "up:\n\ndown:\n";

		// create the empty file
		FILE *file = fopen(full_name, "ab+");
		fwrite(template, 1, sizeof(template) + 3, file);
		fclose(file);

		printf("created new migration: %s\n", full_name);
	}

	if (strcmp(command, "migrate") == 0) {
		// find and run all migrations
		i = 0;
		infinite {
			if (local_mig[i] == NULL) break;

			if (remote_mig[i] == 0) {
				printf("running migration: %s\n", local_mig[i]);

				// read the migration file
				char path[1024] = "";
				strcat(path, migration_path);
				strcat(path, local_mig[i]);

				long mig_size;
				char *mig = read_file(path, &mig_size);

				char *up = malloc(mig_size + 1);
				char *down = malloc(mig_size + 1);

				if (up == NULL || down == NULL) {
					printf("memory allocation error\n\n");
					exit(1);
				}

				populate_up_down(mig, up, down);

				run_migs(up, mig_size);

				char update_query[100] = "insert into zzzzzbmigmigrations values('";
				strcat(update_query, local_mig[i]);
				strcat(update_query, "');");

				connection = get_mysql_conn();
				mysql_query(connection, update_query);
				mysql_close(connection);

				printf("running migration: %s \033[0;32m-- done \033[0m\n", local_mig[i]);

				free(mig);
				free(up);
				free(down);
			}

			++i;
		}
	}

	if (strcmp(command, "rollback") == 0) {
		// rollback last migration
		int last_mig = -1;

		i = 0;
		infinite {
			if (local_mig[i] == NULL) break;

			if (remote_mig[i] == 1) {
				last_mig = i;
			}

			++i;
		}

		if (last_mig == -1) {
			printf("nothing to rollback\n\n");
			return 1;
		}

		printf("rolling back migration: %s\n", local_mig[last_mig]);

		// read the migration file
		char path[1024] = "";
		strcat(path, migration_path);
		strcat(path, local_mig[last_mig]);

		long mig_size;
		char *mig = read_file(path, &mig_size);

		char *up = malloc(mig_size + 1);
		char *down = malloc(mig_size + 1);

		if (up == NULL || down == NULL) {
			printf("memory allocation error\n\n");
			exit(1);
		}

		populate_up_down(mig, up, down);

		run_migs(down, mig_size);

		char update_query[100] = "delete from zzzzzbmigmigrations where name='";
		strcat(update_query, local_mig[last_mig]);
		strcat(update_query, "';");

		connection = get_mysql_conn();
		mysql_query(connection, update_query);
		mysql_close(connection);

		printf("rolling back migration: %s \033[0;32m-- done \033[0m\n", local_mig[last_mig]);

		free(mig);
		free(up);
		free(down);
	}

	// clean up local mig pointers
	int j;
	for (j = 0;; ++j) {
		if (local_mig[j] == '\0') break;

		free(local_mig[j]);
	}

	printf("\n");
	return 0;
}
