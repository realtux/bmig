migrator: clean
	gcc src/bmig.c \
		src/config.c \
		src/mysql.c \
		src/util.c \
		-o bmig \
		`mysql_config --cflags --libs` \
		`pkg-config --cflags --libs json-c` \
		-std=c99 \
		-Wall \
		-Wextra \
		-Wundef \
		-Wfloat-equal \
		-Wpointer-arith \
		-Wcast-align \
		-Wstrict-prototypes \
		-Wno-missing-field-initializers \
		-Warray-bounds \
		-pedantic \
		-fstrict-aliasing

install:
	cp bmig /usr/local/bin

clean:
	rm -f bmig
