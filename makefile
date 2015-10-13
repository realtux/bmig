migrator: clean
	gcc src/migrator.c -g \
		src/mysql.c \
		src/util.c \
		-o migrator \
		`mysql_config --cflags --libs`

clean:
	rm -f migrator
