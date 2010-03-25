BIN = xml

SRC = \
xml_document.c \
xml_node.c \
main.c

OBJ = \
xml_document.o \
xml_node.o \
main.o

CC  = /usr/bin/gcc
DEPENDFILE = .depend
CFLAGS  = -g -Wall\
 `pkg-config --cflags --libs glib-2.0 gobject-2.0 gthread-2.0 `
LDFLAGS = -lm -lpthread \
 `pkg-config --cflags --libs glib-2.0 gobject-2.0 gthread-2.0 `

vexterm: $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dep: $(SRC)
	$(CC) -MM $(SRC) > $(DEPENDFILE)

clean:
	rm -f $(BIN) $(OBJ)

halfclean:
	rm -f $(OBJ)

-include $(DEPENDFILE)

