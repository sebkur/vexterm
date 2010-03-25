BIN = preftest 

SRC = \
../../gtkplus/seqbox/gtk_seq_box.c \
../../gtkplus/seqbox/gtk_v_seq_box.c \
../../gtkplus/dialog/choose_name_dialog.c \
preferences.c \
colour/colour_area.c \
colour/colour_button.c \
colour/colour_button_auto.c \
colour/colour_palette.c \
colour/marshal.c \
colour_schemes_editor.c \
list_v_box.c \
profile_editor.c \
profiles_editor.c \
settings_editor.c \
../config/marshal.c \
../config/vex_config.c \
../config/vex_config_reader.c \
../config/vex_colour_scheme.c \
../config/vex_profile.c \
../vex_paths.c \
../../libvexterm/terminal_colour_palette.c \
../../helpers/customio.c \
../../helpers/util.c \
../../helpers/tool.c \
main.c

OBJ = \
../../gtkplus/seqbox/gtk_seq_box.o \
../../gtkplus/seqbox/gtk_v_seq_box.o \
../../gtkplus/dialog/choose_name_dialog.o \
preferences.o \
colour/colour_area.o \
colour/colour_button.o \
colour/colour_button_auto.o \
colour/colour_palette.o \
colour/marshal.o \
colour_schemes_editor.o \
profile_editor.o \
profiles_editor.o \
settings_editor.o \
list_v_box.o \
../config/marshal.o \
../config/vex_config.o \
../config/vex_config_reader.o \
../config/vex_colour_scheme.o \
../config/vex_profile.o \
../vex_paths.o \
../../libvexterm/terminal_colour_palette.o \
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

