migrator: clean
	gcc src/bmig.c \
		src/config.c \
		src/mysql.c \
		src/util.c \
		-o bmig \
		`mysql_config --cflags --libs` \
		`pkg-config --cflags --libs json-c`

install:
	sudo cp bmig /usr/local/bin

clean:
	rm -f bmig
	sudo rm -f /usr/local/bin/bmig
