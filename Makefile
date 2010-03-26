BIN = vexterm

SRC = \
src/libvexterm/csi.c \
src/libvexterm/debugger_control.c \
src/libvexterm/history.c \
src/libvexterm/iso2022.c \
src/libvexterm/marshal.c \
src/libvexterm/terminal.c \
src/libvexterm/terminal_config.c \
src/libvexterm/terminal_colour_palette.c \
src/libvexterm/terminal_debugger.c \
src/libvexterm/terminal_handler.c \
src/libvexterm/terminal_status_bar.c \
src/libvexterm/terminal_widget.c \
src/vexterm/config/marshal.c \
src/vexterm/config/vex_colour_scheme.c \
src/vexterm/config/vex_config.c \
src/vexterm/config/vex_config_reader.c \
src/vexterm/config/vex_config_writer.c \
src/vexterm/config/vex_layered_config.c \
src/vexterm/config/vex_profile.c \
src/vexterm/vex_single.c \
src/vexterm/vex_single_container.c \
src/vexterm/vex_term.c \
src/vexterm/vex_paths.c \
src/vexterm/menu.c \
src/vexterm/about/about_widget.c \
src/vexterm/about/about_logo.c \
src/vexterm/about/srt_parser.c \
src/helpers/process.c \
src/helpers/tool.c \
src/helpers/util.c \
src/helpers/customio.c \
src/helpers/stringbuffer/string_buffer.c \
src/helpers/xmlwriter/xml_document.c \
src/helpers/xmlwriter/xml_node.c \
src/gtkplus/seqbox/gtk_seq_box.c \
src/gtkplus/seqbox/gtk_v_seq_box.c \
src/gtkplus/dialog/choose_name_dialog.c \
src/vexterm/preferences/preferences.c \
src/vexterm/preferences/colour/colour_area.c \
src/vexterm/preferences/colour/colour_button.c \
src/vexterm/preferences/colour/colour_button_auto.c \
src/vexterm/preferences/colour/colour_palette.c \
src/vexterm/preferences/colour/marshal.c \
src/vexterm/preferences/colour_schemes_editor.c \
src/vexterm/preferences/list_v_box.c \
src/vexterm/preferences/profile_editor.c \
src/vexterm/preferences/profiles_editor.c \
src/vexterm/preferences/settings_editor.c \
src/main.c

OBJ = \
src/libvexterm/csi.o \
src/libvexterm/debugger_control.o \
src/libvexterm/history.o \
src/libvexterm/iso2022.o \
src/libvexterm/marshal.o \
src/libvexterm/terminal.o \
src/libvexterm/terminal_config.o \
src/libvexterm/terminal_colour_palette.o \
src/libvexterm/terminal_debugger.o \
src/libvexterm/terminal_handler.o \
src/libvexterm/terminal_status_bar.o \
src/libvexterm/terminal_widget.o \
src/vexterm/config/marshal.o \
src/vexterm/config/vex_colour_scheme.o \
src/vexterm/config/vex_config.o \
src/vexterm/config/vex_config_reader.o \
src/vexterm/config/vex_config_writer.o \
src/vexterm/config/vex_layered_config.o \
src/vexterm/config/vex_profile.o \
src/vexterm/vex_single.o \
src/vexterm/vex_single_container.o \
src/vexterm/vex_term.o \
src/vexterm/vex_paths.o \
src/vexterm/menu.o \
src/vexterm/about/about_widget.o \
src/vexterm/about/about_logo.o \
src/vexterm/about/srt_parser.o \
src/helpers/process.o \
src/helpers/tool.o \
src/helpers/util.o \
src/helpers/customio.o \
src/helpers/stringbuffer/string_buffer.o \
src/helpers/xmlwriter/xml_document.o \
src/helpers/xmlwriter/xml_node.o \
src/gtkplus/seqbox/gtk_seq_box.o \
src/gtkplus/seqbox/gtk_v_seq_box.o \
src/gtkplus/dialog/choose_name_dialog.o \
src/vexterm/preferences/preferences.o \
src/vexterm/preferences/colour/colour_area.o \
src/vexterm/preferences/colour/colour_button.o \
src/vexterm/preferences/colour/colour_button_auto.o \
src/vexterm/preferences/colour/colour_palette.o \
src/vexterm/preferences/colour/marshal.o \
src/vexterm/preferences/colour_schemes_editor.o \
src/vexterm/preferences/list_v_box.o \
src/vexterm/preferences/profile_editor.o \
src/vexterm/preferences/profiles_editor.o \
src/vexterm/preferences/settings_editor.o \
src/main.o

CC  = /usr/bin/gcc
DEPENDFILE = .depend
CFLAGS  = -g -Wall\
 `pkg-config --cflags --libs gtk+-2.0 gdk-2.0 pango cairo glib-2.0 gthread-2.0 `
LDFLAGS = -lm -lpthread -lpopt -lexpat\
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

