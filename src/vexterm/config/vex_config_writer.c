/* VexTerm - a lightweight and fast terminal emulator
 *
 * Copyright (C) 2010  Sebastian Kuerten
 *
 * This file is part of VexTerm.
 *
 * VexTerm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VexTerm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VexTerm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_config_writer.h"
#include "../../helpers/xmlwriter/xml_document.h"
#include "../../helpers/xmlwriter/xml_node.h"

G_DEFINE_TYPE (VexConfigWriter, vex_config_writer, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint vex_config_writer_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, vex_config_writer_signals[SIGNAL_NAME_n], 0);

VexConfigWriter * vex_config_writer_new()
{
	VexConfigWriter * vex_config_writer = g_object_new(VEX_TYPE_VEX_CONFIG_WRITER, NULL);
	return vex_config_writer;
}

static void vex_config_writer_class_init(VexConfigWriterClass *class)
{
        /*vex_config_writer_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexConfigWriterClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void vex_config_writer_init(VexConfigWriter *vex_config_writer)
{
}

void vex_config_writer_write_config_to_file(VexConfigWriter * vcw, char * file, VexConfig * config)
{
	XmlDocument * doc = xml_document_new();
	XmlNode * configuration = xml_node_new("configuration");
	xml_document_set_root(doc, configuration);

	XmlNode * preferences = xml_node_new("preferences");
	xml_node_add_child(configuration, preferences);
	xml_node_add_tag(preferences, "profile", vex_config_get_active_profile(config));

	XmlNode * debug = xml_node_new("debug");
	xml_node_add_child(preferences, debug);
	xml_node_add_tag(debug, "scrolling-region", vex_config_get_show_scrolling_region(config) ? "yes" : "no");
	xml_node_add_tag(debug, "status-bar", vex_config_get_show_status_bar(config) ? "yes" : "no");

	XmlNode * tabs = xml_node_new("tabs");
	xml_node_add_child(preferences, tabs);
	GtkPositionType tabs_pos = vex_config_get_tabs_position(config);
	char * tabs_pos_s = "top";
	switch(tabs_pos){
		case GTK_POS_TOP:{
			tabs_pos_s = "top";
			break;
		}
		case GTK_POS_BOTTOM:{
			tabs_pos_s = "bottom";
			break;
		}
		case GTK_POS_LEFT:{
			tabs_pos_s = "left";
			break;
		}
		case GTK_POS_RIGHT:{
			tabs_pos_s = "right";
			break;
		}
	}
	xml_node_add_tag(tabs, "position", tabs_pos_s);

	XmlNode * profiles = xml_node_new("profiles");
	xml_node_add_child(configuration, profiles);

	int i;
	for (i = 0; i < vex_config_get_number_of_profiles(config); i++){
		char * profile_name;
		VexProfile * profile = vex_config_get_profile_by_index(config, i, &profile_name);

		XmlNode * profile_node = xml_node_new("profile");
		xml_node_add_child(profiles, profile_node);
		xml_node_add_tag(profile_node, "name", profile_name);

		XmlNode * font_node = xml_node_new("font");
		xml_node_add_child(profile_node, font_node);
		xml_node_add_tag(font_node, "name", vex_profile_get_font_name(profile));
		char * size = g_strdup_printf("%d", vex_profile_get_font_size(profile));
		xml_node_add_tag(font_node, "size", size);
		free(size);

		XmlNode * colours_node = xml_node_new("colours");
		xml_node_add_child(profile_node, colours_node);
		xml_node_add_tag(colours_node, "scheme", vex_profile_get_active_colour_scheme_name(profile));
	}

	XmlNode * colourschemes = xml_node_new("colourschemes");
	xml_node_add_child(configuration, colourschemes);

	for (i = 0; i < vex_config_get_number_of_colour_schemes(config); i++){
		char * cs_name;
		VexColourScheme * cs = vex_config_get_colour_scheme_by_index(config, i, &cs_name);

		XmlNode * cs_node = xml_node_new("colourscheme");
		xml_node_add_child(colourschemes, cs_node);
		xml_node_add_tag(cs_node, "name", cs_name);

		int c;
		for (c = 0; c < 18; c++){
			VexColour * colour = vex_colour_scheme_get_colour(cs, c);
			XmlNode * colour_node = xml_node_new("colour");
			xml_node_add_child(cs_node, colour_node);
			xml_node_add_tag(colour_node, "encoding", "hex16");
			char * sr = g_strdup_printf("0x%04X", colour -> r);
			char * sg = g_strdup_printf("0x%04X", colour -> g);
			char * sb = g_strdup_printf("0x%04X", colour -> b);
			char * sa = g_strdup_printf("0x%04X", colour -> a);
			xml_node_add_tag(colour_node, "r", sr);
			xml_node_add_tag(colour_node, "g", sg);
			xml_node_add_tag(colour_node, "b", sb);
			xml_node_add_tag(colour_node, "a", sa);
			free(sr); free(sg); free(sb); free(sa);
		}
	}

	int fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd > 0){
		xml_document_write(doc, fd);
		close(fd);
	}else{
		printf("WARNING: unable to write config file\n");
	}
}
