BIN = seqboxtest

SRC = \
gtk_seq_box.c \
gtk_v_seq_box.c \
../../helpers/customio.c \
../../helpers/util.c \
../../helpers/tool.c \
main.c

OBJ = \
gtk_seq_box.o \
gtk_v_seq_box.o \
../../helpers/customio.o \
../../helpers/util.o \
../../helpers/tool.o \
main.o

CC  = /usr/bin/gcc
DEPENDFILE = .depend
CFLAGS  = -g -Wall\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0 `
LDFLAGS = -lm -lpthread -lexpat\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0 `

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

