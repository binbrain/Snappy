SHELL = /bin/sh
SOURCES = src/card.c src/cardstore.c src/category.c src/catalog.c src/cardwidget.c src/util.c src/categories.c src/preferences.c src/editor.c src/gui.c src/about.c src/main.c
OBJS    = ${SOURCES:.c=.o}
CFLAGS  = `pkg-config gtk+-2.0 sqlite3 libglade-2.0 --cflags` -g
LDADD   = `pkg-config gtk+-2.0 sqlite3 libglade-2.0 --libs`
CC      = gcc -ggdb
PACKAGE = snappy


all : ${OBJS}
	${CC} -g -o ${PACKAGE} ${OBJS} ${LDADD}

src/.c.o:
	${CC} ${CFLAGS} -c $<

memcheck: 
	valgrind --leak-check=yes ./snappy

clean:
	rm -f src/*.o ${PACKAGE}

# end of file
