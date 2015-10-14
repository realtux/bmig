migrator: clean
	gcc src/bmig.c \
		src/config.c \
		src/mysql.c \
		src/util.c \
		-o bmig \
		`mysql_config --cflags --libs` \
		`pkg-config --cflags --libs json-c` \
		-Wall \
		-Wextra \
		-Wundef \
		-pedantic \
		-std=c99 \
		-Wstrict-prototypes \
		-Wno-missing-field-initializers \
		-Wstrict-aliasing=3 \
		-fstrict-aliasing \
		-Warray-bounds

install:
	sudo cp bmig /usr/local/bin

clean:
	rm -f bmig
