BIN = splittable

SRC = \
gtk_splittable.c \
splittable_collection.c \
../../libvexterm/csi.c \
../../libvexterm/debugger_control.c \
../../libvexterm/history.c \
../../libvexterm/iso2022.c \
../../libvexterm/marshal.c \
../../libvexterm/terminal.c \
../../libvexterm/terminal_config.c \
../../libvexterm/terminal_colour_palette.c \
../../libvexterm/terminal_debugger.c \
../../libvexterm/terminal_handler.c \
../../libvexterm/terminal_status_bar.c \
../../libvexterm/terminal_widget.c \
../../vexterm/config/marshal.c \
../../vexterm/config/vex_colour_scheme.c \
../../vexterm/config/vex_config.c \
../../vexterm/config/vex_layered_config.c \
../../vexterm/config/vex_profile.c \
../../vexterm/vex_single.c \
../../vexterm/vex_single_container.c \
../../vexterm/vex_paths.c \
../../vexterm/menu.c \
../../vexterm/about/about_widget.c \
../../vexterm/about/about_logo.c \
../../vexterm/about/srt_parser.c \
../../helpers/g_map/g_map.c \
../../helpers/process.c \
../../helpers/tool.c \
../../helpers/util.c \
../../helpers/customio.c \
../../helpers/stringbuffer/string_buffer.c \
main.c

OBJ = \
gtk_splittable.o \
splittable_collection.o \
../../libvexterm/csi.o \
../../libvexterm/debugger_control.o \
../../libvexterm/history.o \
../../libvexterm/iso2022.o \
../../libvexterm/marshal.o \
../../libvexterm/terminal.o \
../../libvexterm/terminal_config.o \
../../libvexterm/terminal_colour_palette.o \
../../libvexterm/terminal_debugger.o \
../../libvexterm/terminal_handler.o \
../../libvexterm/terminal_status_bar.o \
../../libvexterm/terminal_widget.o \
../../vexterm/config/marshal.o \
../../vexterm/config/vex_colour_scheme.o \
../../vexterm/config/vex_config.o \
../../vexterm/config/vex_layered_config.o \
../../vexterm/config/vex_profile.o \
../../vexterm/vex_single.o \
../../vexterm/vex_single_container.o \
../../vexterm/vex_paths.o \
../../vexterm/menu.o \
../../vexterm/about/about_widget.o \
../../vexterm/about/about_logo.o \
../../vexterm/about/srt_parser.o \
../../helpers/g_map/g_map.o \
../../helpers/process.o \
../../helpers/tool.o \
../../helpers/util.o \
../../helpers/customio.o \
../../helpers/stringbuffer/string_buffer.o \
main.o

CC  = /usr/bin/gcc
DEPENDFILE = .depend
CFLAGS  = -g -Wall\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0 `
LDFLAGS = -lm -lpthread \
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

