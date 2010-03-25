#!/usr/bin/python

import sys
import string

copyright =	"/* VexTerm - a lightweight and fast terminal emulator\n" + \
		" *\n" + \
		" * Copyright (C) 2010  Sebastian Kuerten\n" + \
		" *\n" + \
		" * This file is part of VexTerm.\n" + \
		" *\n" + \
		" * VexTerm is free software: you can redistribute it and/or modify\n" + \
		" * it under the terms of the GNU General Public License as published by\n" + \
		" * the Free Software Foundation, either version 3 of the License, or\n" + \
		" * (at your option) any later version.\n" + \
		" *\n" + \
		" * VexTerm is distributed in the hope that it will be useful,\n" + \
		" * but WITHOUT ANY WARRANTY; without even the implied warranty of\n" + \
		" * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" + \
		" * GNU General Public License for more details.\n" + \
		" *\n" + \
		" * You should have received a copy of the GNU General Public License\n" + \
		" * along with VexTerm.  If not, see <http://www.gnu.org/licenses/>.\n" + \
		" */\n"

def get_lower_name(name):
	comps = typename.split("_");
	comps1 = map(string.lower, comps)
	return reduce(lambda x, y: x + "_" + y, comps1)

def get_upper_name(name):
	comps = typename.split("_");
	comps2 = map(string.upper, comps)
	return reduce(lambda x, y: x + "_" + y, comps2)

def get_mixed_name(name):
	comps = typename.split("_");
	comps3 = map(lambda x: x[0].upper() + x[1:], comps)
	return reduce(lambda x, y: x + y, comps3)

def get_names(name):
	return [get_lower_name(name), get_upper_name(name), get_mixed_name(name)]

def get_header(name, parent_mixed, type_return):
	names = get_names(name)
	text_h = \
	"#ifndef _"+names[1]+"_H_\n" + \
	"#define _"+names[1]+"_H_\n" + \
	"\n" + \
	"#include <stdio.h>\n" + \
	"#include <stdlib.h>\n" + \
	"#include <unistd.h>\n" + \
	"\n" + \
	"#include <glib.h>\n" + \
	"#include <gtk/gtk.h>\n" + \
	"#include <gdk/gdk.h>\n" + \
	"\n" + \
	"#define VEX_TYPE_"+names[1]+"           ("+names[0]+"_get_type ())\n" + \
	"#define VEX_"+names[1]+"(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_"+names[1]+", "+names[2]+"))\n" + \
	"#define VEX_"+names[1]+"_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_"+names[1]+", "+names[2]+"Class))\n" + \
	"#define VEX_IS_"+names[1]+"(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_"+names[1]+"))\n" + \
	"#define VEX_IS_"+names[1]+"_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_"+names[1]+"))\n" + \
	"#define VEX_"+names[1]+"_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_"+names[1]+", "+names[2]+"Class))\n" + \
	"\n" + \
	"GType "+names[0]+"_get_type (void) G_GNUC_CONST; \n" + \
	"\n" + \
	"typedef struct _"+names[2]+"        "+names[2]+";\n" + \
	"typedef struct _"+names[2]+"Class   "+names[2]+"Class;\n" + \
	"\n" + \
	"struct _"+names[2]+"\n" + \
	"{\n" + \
	"	"+parent_mixed+" parent;\n" + \
	"};\n" + \
	"\n" + \
	"struct _"+names[2]+"Class\n" + \
	"{\n" + \
	"	"+parent_mixed+"Class parent_class;\n" + \
	"\n" + \
	"	//void (* function_name) ("+names[2]+" *"+names[0]+");\n" + \
	"};\n" + \
	"\n" + \
	type_return+" * "+names[0]+"_new();\n" + \
	"\n" + \
	"#endif /* _"+names[1]+"_H_ */\n"
	return text_h

def get_class(name, parent_mixed, parent_upper, type_return):
	names = get_names(name)
	text_c = \
	"#include <stdio.h>\n" + \
	"#include <stdlib.h>\n" + \
	"#include <unistd.h>\n" + \
	"\n" + \
	"#include <glib.h>\n" + \
	"#include <gtk/gtk.h>\n" + \
	"#include <gdk/gdk.h>\n" + \
	"\n" + \
	"#include \""+names[0]+".h\"\n" + \
	"\n" + \
	"G_DEFINE_TYPE ("+names[2]+", "+names[0]+", "+parent_upper+");\n" + \
	"\n" + \
	"/*enum\n" + \
	"{\n" + \
	"        SIGNAL_NAME_1,\n" + \
	"        SIGNAL_NAME_n,\n" + \
	"        LAST_SIGNAL\n" + \
	"};*/\n" + \
	"\n" + \
	"//static guint "+names[0]+"_signals[LAST_SIGNAL] = { 0 };\n" + \
	"//g_signal_emit (widget, "+names[0]+"_signals[SIGNAL_NAME_n], 0);\n" + \
	"\n" + \
	type_return+" * "+names[0]+"_new()\n" + \
	"{\n" + \
	"	"+names[2]+" * "+names[0]+" = g_object_new(VEX_TYPE_"+names[1]+", NULL);\n" + \
	"	return "+names[0]+";\n" + \
	"}\n" + \
	"\n" + \
	"static void "+names[0]+"_class_init("+names[2]+"Class *class)\n" + \
	"{\n" + \
	"        /*"+names[0]+"_signals[SIGNAL_NAME_n] = g_signal_new(\n" + \
	"                \"signal-name-n\",\n" + \
	"                G_OBJECT_CLASS_TYPE (class),\n" + \
	"                G_SIGNAL_RUN_FIRST,\n" + \
	"                G_STRUCT_OFFSET ("+names[2]+"Class, function_name),\n" + \
	"                NULL, NULL,\n" + \
	"                g_cclosure_marshal_VOID__VOID,\n" + \
	"                G_TYPE_NONE, 0);*/\n" + \
	"}\n" + \
	"\n" + \
	"static void "+names[0]+"_init("+names[2]+" *"+names[0]+")\n" + \
	"{\n" + \
	"}\n"
	return text_c

args = len(sys.argv)
if not (args == 2 or args == 4 or args == 5):
	print "usage: create_type.py <a_custom_name> [<ParentType> <TYPE_PARENT> [<ReturnType>]]"
	print "example: create_type.py my_name"
	print "example: create_type.py my_name GtkVBox GTK_TYPE_VBOX"
	print "example: create_type.py my_name GtkVBox GTK_TYPE_VBOX GtkWidget"
	exit()	

# typename is 'my_custom_name'
typename = sys.argv[1]
names = get_names(typename)

parent_mixed = "GObject"
parent_upper = "G_TYPE_OBJECT"
if args >= 4:
	parent_mixed = sys.argv[2]
	parent_upper = sys.argv[3]
type_return = parent_mixed
if args == 5:
	type_return = sys.argv[4]

text_h = get_header(typename, parent_mixed, type_return)
text_c = get_class(typename, parent_mixed, parent_upper, type_return)

text_h = copyright + "\n" + text_h
text_c = copyright + "\n" + text_c

print "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
print text_h
print "HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"
print "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
print text_c
print "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
print
print "write", "'"+names[0]+".h'", "and", "'"+names[0]+".c'", "?"
confirmation = sys.stdin.readline()[0]
if not confirmation == 'y':
	print "abort"
	exit()	

f_h = open(names[0]+".h", "w")
f_c = open(names[0]+".c", "w")
f_h.write(text_h)
f_c.write(text_c)
f_h.close()
f_c.close()
