#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <mysql.h>
#include <string.h>
#include <ctype.h>

#include "util.h"
#include "mysql.h"

#define VERSION "0.0.1"

void menu() {
	printf("usage: ./migrator command\n");
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
	printf("    rollback\n");
	printf("        rollback the last migration\n");
	printf("\n");
}

void populate_local_mig(char **local_mig) {
	// open parent dir
	DIR *dir;
	struct dirent *directory;

	dir = opendir("../");

	size_t i = 0;

	// construct local_mig full of .sql files
	while ((directory = readdir(dir)) != NULL) {
		char *dir_name = directory->d_name;
		size_t len = strlen(dir_name);

		// add only .sql files to the local_mig
		if (strcmp(dir_name + len - 4, ".sql") == 0) {
			local_mig[i] = dir_name;
			++i;
		}
	}

	qsort(local_mig, i, sizeof(char *), cstring_cmp);
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

	// determine up end based on whether down is present
	up_end = down_start >= 0 ? down_start - 7 : strlen(mig);

	// determine down end based on whether up is present
	down_end = down_start >= 0 ? strlen(mig) : -1;

	// load bodies
	size_t x = 0;
	size_t y = 0;

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

int main(int argc, char **argv) {
	printf("bmig version %s\n", VERSION);

	// no command, needs a menu
	if (argc < 2) {
		menu();
		return 0;
	}

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
	get_remote_status(connection, local_mig, &remote_mig);

	mysql_close(connection);

	// determine command
	if (strcmp(command, "status") == 0) {
		// check local_mig against remote_mig for differences
		i = 0;
		for (;;) {
			if (local_mig[i] == NULL) break;

			if (remote_mig[i] == 1) {
				printf("\e[0;32mup - \e[0;37m");
			} else {
				printf("\e[0;31mdn - \e[0;37m");
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

		char full_name[1024] = "../";

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
		for (;;) {
			if (local_mig[i] == NULL) break;

			if (remote_mig[i] == 0) {
				printf("running migration: %s", local_mig[i]);

				// read the migration file
				char path[1024] = "";
				strcat(path, "../");
				strcat(path, local_mig[i]);

				char *mig = read_file(path);

				char *up = malloc(3000000);
				char *down = malloc(3000000);

				populate_up_down(mig, up, down);

				// run the up
				connection = get_mysql_conn();
				mysql_query(connection, up);
				mysql_close(connection);

				char update_query[100] = "insert into zzzzzbmigmigrations values('";
				strcat(update_query, local_mig[i]);
				strcat(update_query, "');");

				connection = get_mysql_conn();
				mysql_query(connection, update_query);
				mysql_close(connection);

				printf("\rrunning migration: %s -- done\n", local_mig[i]);

				free(mig);
				free(up);
				free(down);
			}

			++i;
		}
	}

	if (strcmp(command, "rollback") == 0) {
		// rollback last migration
		size_t last_mig = -1;

		i = 0;
		for (;;) {
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

		printf("rolling back migration: %s", local_mig[last_mig]);

		// read the migration file
		char path[1024] = "";
		strcat(path, "../");
		strcat(path, local_mig[last_mig]);

		char *mig = read_file(path);

		char *up = malloc(3000000);
		char *down = malloc(3000000);

		populate_up_down(mig, up, down);

		connection = get_mysql_conn();
		mysql_query(connection, down);
		mysql_close(connection);

		char update_query[100] = "delete from zzzzzbmigmigrations where name='";
		strcat(update_query, local_mig[last_mig]);
		strcat(update_query, "';");

		connection = get_mysql_conn();
		mysql_query(connection, update_query);
		mysql_close(connection);

		printf("\rrolling back migration: %s -- done\n", local_mig[last_mig]);
	}

	printf("\n");
	return 0;
}
