migrator: clean
	gcc src/migrator.c -g \
		src/config.c \
		src/mysql.c \
		src/util.c \
		-o bmig \
		`mysql_config --cflags --libs` \
		`pkg-config --cflags --libs json-c`

clean:
	rm -f bmig
