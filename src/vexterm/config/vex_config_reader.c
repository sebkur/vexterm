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

#define DEBUG_MAIN 0
#define DEBUG_XML 0
#define DEBUG_ITEMS 1
#define DEBUG_COLOURS 0

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <expat.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_config_reader.h"

G_DEFINE_TYPE (VexConfigReader, vex_config_reader, G_TYPE_OBJECT);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint vex_config_reader_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, vex_config_reader_signals[SIGNAL_NAME_n], 0);

VexConfigReader * vex_config_reader_new()
{
	VexConfigReader * vex_config_reader = g_object_new(LIBVEX_TYPE_VEX_CONFIG_READER, NULL);
	return vex_config_reader;
}

static void vex_config_reader_class_init(VexConfigReaderClass *class)
{
        /*vex_config_reader_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexConfigReaderClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

typedef enum{
	TAG_NONE,
	TAG_UNKNOWN,
	TAG_CONFIGURATION,
	  TAG_PREFERENCES,
	    TAG_DEBUG,
	    TAG_TABS,
	  TAG_PROFILES,
	    TAG_PROFILE,
	      TAG_FONT,
	      TAG_COLOURS,
	  TAG_COLOUR_SCHEMES,
	    TAG_COLOUR_SCHEME,
	      TAG_COLOUR
} Tags;

#define TAG_IS(tag) (strcmp(tag, name) == 0)

static void vex_config_reader_init(VexConfigReader * vcr)
{
	vcr -> nesting = g_array_new(FALSE, FALSE, sizeof(gpointer));
	vcr -> colour_scheme_name = NULL;
	vcr -> colour_scheme_colour_count = 0;
}

void vex_config_reader_nesting_push(VexConfigReader * vcr, Tags tag)
{
	gpointer p = GINT_TO_POINTER(tag);
	g_array_append_val(vcr -> nesting, p);
}

Tags vex_config_reader_nesting_top(VexConfigReader * vcr)
{
	int l = vcr -> nesting -> len;
	if (l == 0) return TAG_NONE;
	gpointer tagp = g_array_index(vcr -> nesting, gpointer, l - 1);
	Tags tag = GPOINTER_TO_INT(tagp);
	return tag;
}

Tags vex_config_reader_nesting_pop(VexConfigReader * vcr)
{
	int l = vcr -> nesting -> len;
	if (l == 0) return TAG_NONE;
	gpointer tagp = g_array_index(vcr -> nesting, gpointer, l - 1);
	Tags tag = GPOINTER_TO_INT(tagp);
	g_array_remove_index(vcr -> nesting, l - 1);
	return tag;
}

const char * vex_config_reader_get_value(const XML_Char ** atts, char * search_key)
{
	const XML_Char ** ptr = atts;
	while(*ptr != NULL){
		const XML_Char * key = *ptr;
		const XML_Char * value = *(ptr + 1);
		if (strcmp(key, search_key) == 0){
			return value;
		}
		ptr = ptr + 2;
	}
	return NULL;
}

void vex_config_reader_handle_preferences(VexConfigReader * vcr, const XML_Char ** atts);
void vex_config_reader_handle_profile(VexConfigReader * vcr, const XML_Char ** atts);
void vex_config_reader_handle_font(VexConfigReader * vcr, const XML_Char ** atts);
void vex_config_reader_handle_colours(VexConfigReader * vcr, const XML_Char ** atts);
void vex_config_reader_handle_debug(VexConfigReader * vcr, const XML_Char ** atts);
void vex_config_reader_handle_tabs(VexConfigReader * vcr, const XML_Char ** atts);
void vex_config_reader_handle_colour_scheme(VexConfigReader * vcr, const XML_Char ** atts);
void vex_config_reader_handle_colour(VexConfigReader * vcr, const XML_Char ** atts);
void vex_config_reader_handle_profile_end(VexConfigReader * vcr);
void vex_config_reader_handle_colour_scheme_end(VexConfigReader * vcr);

static void XMLCALL vex_config_reader_StartElementCallback(
						void * userData,
						const XML_Char * name,
						const XML_Char ** atts)
{
	VexConfigReader * vcr = (VexConfigReader*)userData;
	#if DEBUG_XML
	printf("start %s\n", name);
	#endif

	Tags tag = vex_config_reader_nesting_top(vcr);
	switch(tag){
		case TAG_NONE:{
			if (TAG_IS("configuration")){
				vex_config_reader_nesting_push(vcr, TAG_CONFIGURATION);
			}else{
				vex_config_reader_nesting_push(vcr, TAG_UNKNOWN);
			}
			break;
		}
		case TAG_CONFIGURATION:{
			if (TAG_IS("preferences")){
				vex_config_reader_nesting_push(vcr, TAG_PREFERENCES);
				vex_config_reader_handle_preferences(vcr, atts);
			}else if(TAG_IS("profiles")){
				vex_config_reader_nesting_push(vcr, TAG_PROFILES);
			}else if(TAG_IS("colourschemes")){
				vex_config_reader_nesting_push(vcr, TAG_COLOUR_SCHEMES);
			}else{
				vex_config_reader_nesting_push(vcr, TAG_UNKNOWN);
			}
			break;
		}
		case TAG_PREFERENCES:{
			if(TAG_IS("debug")){
				vex_config_reader_nesting_push(vcr, TAG_DEBUG);
				vex_config_reader_handle_debug(vcr, atts);
			}else if(TAG_IS("tabs")){
				vex_config_reader_nesting_push(vcr, TAG_TABS);
				vex_config_reader_handle_tabs(vcr, atts);
			}else{
				vex_config_reader_nesting_push(vcr, TAG_UNKNOWN);
			}
			break;
		}
		case TAG_PROFILES:{
			if (TAG_IS("profile")){
				vex_config_reader_nesting_push(vcr, TAG_PROFILE);
				vex_config_reader_handle_profile(vcr, atts);
			}else{
				vex_config_reader_nesting_push(vcr, TAG_UNKNOWN);
			}
			break;
		}
		case TAG_PROFILE:{
			if (TAG_IS("font")){
				vex_config_reader_nesting_push(vcr, TAG_FONT);
				vex_config_reader_handle_font(vcr, atts);
			}else if(TAG_IS("colours")){
				vex_config_reader_nesting_push(vcr, TAG_COLOURS);
				vex_config_reader_handle_colours(vcr, atts);
			}else{
				vex_config_reader_nesting_push(vcr, TAG_UNKNOWN);
			}
			break;
		}
		case TAG_COLOUR_SCHEMES:{
			if (TAG_IS("colourscheme")){
				vex_config_reader_nesting_push(vcr, TAG_COLOUR_SCHEME);
				vex_config_reader_handle_colour_scheme(vcr, atts);
			}else{
				vex_config_reader_nesting_push(vcr, TAG_UNKNOWN);
			}
			break;
		}
		case TAG_COLOUR_SCHEME:{
			if (TAG_IS("colour")){
				vex_config_reader_nesting_push(vcr, TAG_COLOUR);
				vex_config_reader_handle_colour(vcr, atts);
			}else{
				vex_config_reader_nesting_push(vcr, TAG_UNKNOWN);
			}
			break;
		}
		case TAG_FONT:
		case TAG_COLOURS:
		case TAG_DEBUG:
		case TAG_COLOUR:{
			// not expexting any child nodes here
			vex_config_reader_nesting_push(vcr, TAG_UNKNOWN);
			break;
		}
		default:{
			// UNKNOWN
			vex_config_reader_nesting_push(vcr, TAG_UNKNOWN);
			break;
		}
	}
}

static void XMLCALL vex_config_reader_EndElementCallback(
					void * userData,
					const XML_Char * name)
{
	VexConfigReader * vcr = (VexConfigReader*)userData;
	#if DEBUG_XML
	printf("end %s\n", name);
	#endif
	Tags tag = vex_config_reader_nesting_pop(vcr);
	switch(tag){
		case TAG_COLOUR_SCHEME:{
			if(TAG_IS("colourscheme")){
				vex_config_reader_handle_colour_scheme_end(vcr);
			}
			break;
		}
		case TAG_PROFILE:{
			if(TAG_IS("profile")){
				vex_config_reader_handle_profile_end(vcr);
			}
			break;
		}
		default:{
			break;
		}
	}
}

static void XMLCALL vex_config_reader_CharacterDataCallback(
					void * userData,
					const XML_Char * s,
					int len)
{
}


void vex_config_reader_fill_config_from_file(VexConfigReader * vcr, char * file, VexConfig * vex_config)
{
	printf("Filling config from %s\n", file);

	vcr -> vex_config = vex_config;

	struct stat sb;
	int s = stat(file, &sb);
	if (s < 0){
		printf("could not open file, stat failed\n");
		return;
	}
	int f = open(file, O_RDONLY);
	if (f == 0) {
		printf("could not open file, open failed\n");
		return;
	}

	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, vcr);
	XML_SetStartElementHandler(parser, vex_config_reader_StartElementCallback);
	XML_SetEndElementHandler(parser, vex_config_reader_EndElementCallback);
	XML_SetCharacterDataHandler(parser, vex_config_reader_CharacterDataCallback);

	#if DEBUG_MAIN
	printf("start parsing\n");
	#endif

	ssize_t size;
	int n = 1024 * 4;
	char buf[n];
	do{
		size = read(f, buf, n);
		XML_Parse(parser, buf, size, 0);
	}while(size > 0);
	XML_Parse(parser, NULL, 0, 1);
	XML_ParserFree(parser);

	#if DEBUG_MAIN
	printf("end parsing\n");
	#endif
}

/* the following function actually set config values */

void vex_config_reader_handle_preferences(VexConfigReader * vcr, const XML_Char ** atts)
{
	const char * profile = vex_config_reader_get_value(atts, "profile");
	#if DEBUG_ITEMS
	printf("PREFERENCES profile: %s\n", profile);
	#endif
	vex_config_set_active_profile(vcr -> vex_config, (char*)profile);
}

void vex_config_reader_handle_profile(VexConfigReader * vcr, const XML_Char ** atts)
{
	const char * name = vex_config_reader_get_value(atts, "name");
	if (name != NULL){
		#if DEBUG_ITEMS
		printf("PROFILE name: %s\n", name);
		#endif
		vcr -> profile_name = g_strdup(name);
		vcr -> profile = vex_profile_new();
	}
}

void vex_config_reader_handle_font(VexConfigReader * vcr, const XML_Char ** atts)
{
	const char * font_name = vex_config_reader_get_value(atts, "name");
	const char * font_size = vex_config_reader_get_value(atts, "size");
	if (font_name != NULL && font_size != NULL){
		errno = 0;
		int n = strtol(font_size, (char**) NULL, 10);
		if (errno == 0){
			#if DEBUG_ITEMS
			printf("SET FONT name: %s size: %s\n", font_name, font_size);
			#endif
			vex_profile_set_font_name(vcr -> profile, (char*)font_name);
			vex_profile_set_font_size(vcr -> profile, n);
		}
	}
}

void vex_config_reader_handle_colours(VexConfigReader * vcr, const XML_Char ** atts)
{
	const char * scheme_name = vex_config_reader_get_value(atts, "scheme");
	if (scheme_name != NULL){
		#if DEBUG_ITEMS
		printf("SET COLOUR SCHEME name: %s\n", scheme_name);
		#endif
		vex_profile_set_active_colour_scheme_name(vcr -> profile, (char*)scheme_name);
	}
}

void vex_config_reader_handle_debug(VexConfigReader * vcr, const XML_Char ** atts)
{
	const char * scrolling_region = vex_config_reader_get_value(atts, "scrolling-region");
	const char * status_bar = vex_config_reader_get_value(atts, "status-bar");
	if (scrolling_region != NULL){
		gboolean show = strcmp(scrolling_region, "yes") == 0;
		#if DEBUG_ITEMS
		printf("SET SCROLLING REGION: %d\n", show);
		#endif
		vex_config_set_show_scrolling_region(vcr -> vex_config, show);
	}
	if (status_bar != NULL){
		gboolean show = strcmp(status_bar, "yes") == 0;
		#if DEBUG_ITEMS
		printf("SET STATUS BAR: %d\n", show);
		#endif
		vex_config_set_show_status_bar(vcr -> vex_config, show);
	}
}

void vex_config_reader_handle_tabs(VexConfigReader * vcr, const XML_Char ** atts)
{
	const char * position = vex_config_reader_get_value(atts, "position");
	if (position != NULL){
		int pos = -1;
		if (strcmp(position, "top") == 0){
			pos = GTK_POS_TOP;
		}else if(strcmp(position, "bottom") == 0){
			pos = GTK_POS_BOTTOM;
		}else if(strcmp(position, "left") == 0){
			pos = GTK_POS_LEFT;
		}else if(strcmp(position, "right") == 0){
			pos = GTK_POS_RIGHT;
		}
		if (pos != -1){
			vex_config_set_tabs_position(vcr -> vex_config, pos);
		}
	}
}

void vex_config_reader_handle_colour_scheme(VexConfigReader * vcr, const XML_Char ** atts)
{
	const char * name = vex_config_reader_get_value(atts, "name");
	if (name != NULL){
		#if DEBUG_ITEMS
		printf("COLOURSCHEME name: %s\n", name);
		#endif
		vcr -> colour_scheme_name = g_strdup(name);
		vcr -> colour_scheme_colour_count = 0;
		vcr -> colour_scheme = vex_colour_scheme_new();
	}
}

void vex_config_reader_handle_colour(VexConfigReader * vcr, const XML_Char ** atts)
{
	const char * encoding = vex_config_reader_get_value(atts, "encoding");
	if (strcmp(encoding, "hex16") == 0){
		const char * sr = vex_config_reader_get_value(atts, "r");
		const char * sg = vex_config_reader_get_value(atts, "g");
		const char * sb = vex_config_reader_get_value(atts, "b");
		const char * sa = vex_config_reader_get_value(atts, "a");
		char * end;
		gboolean vr, vg, vb, va;
		errno = 0;
		int r = strtol(sr, &end, 16);
		errno = 0;
		vr = errno == 0 && sr != end;
		errno = 0;
		int g = strtol(sg, &end, 16);
		vg = errno == 0 && sg != end;
		errno = 0;
		int b = strtol(sb, &end, 16);
		vb = errno == 0 && sb != end;
		errno = 0;
		int a = strtol(sa, &end, 16);
		va = errno == 0 && sa != end;

		if(vr && vg && vb && va){
			VexColour colour;
			colour.r = r;
			colour.g = g;
			colour.b = b;
			colour.a = a;
			int c = vcr -> colour_scheme_colour_count;
			if (c < 18){
				if (c == 0){
					vcr -> colour_scheme -> foreground = colour;
				}else if(c == 1){
					vcr -> colour_scheme -> background = colour;
				}else if(c < 10){
					vcr -> colour_scheme -> normal[c-2] = colour;
				}else{
					vcr -> colour_scheme -> highlighted[c-10] = colour;
				}
				#if DEBUG_ITEMS && DEBUG_COLOURS
				printf("%d %d %d %d\n", r, g, b, a);
				#endif
			}
			vcr -> colour_scheme_colour_count += 1;
		}
	}
}

void vex_config_reader_handle_colour_scheme_end(VexConfigReader * vcr)
{
	#if DEBUG_ITEMS
	printf("COLOURSCHEME END name: %s, colours: %d\n", 
		vcr -> colour_scheme_name, vcr -> colour_scheme_colour_count);
	#endif
	if (vcr -> colour_scheme_colour_count == 18){
		vex_config_add_colour_scheme(
			vcr -> vex_config,
			vcr -> colour_scheme_name,
			vcr -> colour_scheme
		);
	}
	/* reset */
	g_free(vcr -> colour_scheme_name);
	vcr -> colour_scheme_name = NULL;
	vcr -> colour_scheme_colour_count = 0;
}

void vex_config_reader_handle_profile_end(VexConfigReader * vcr)
{
	#if DEBUG_ITEMS
	printf("PROFILE END name: %s\n", 
		vcr -> profile_name);
	#endif
	if (TRUE){
		vex_config_add_profile(
			vcr -> vex_config,
			vcr -> profile_name,
			vcr -> profile
		);
	}
	/* reset */
	g_free(vcr -> profile_name);
	vcr -> profile_name = NULL;
}
