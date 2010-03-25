#!/usr/bin/python


import sys
import string

#GtkWidget *menubar = gtk_menu_bar_new();
#GtkWidget *item1   = gtk_menu_item_new_with_mnemonic("Options");
#GtkWidget *menu1   = gtk_menu_new();
#GtkWidget *item2   = gtk_menu_item_new_with_mnemonic("Preferences");
#gtk_menu_shell_append((GtkMenuShell*)menubar, item1);
#gtk_menu_item_set_submenu((GtkMenuItem*)item1, menu1);
#gtk_menu_shell_append((GtkMenuShell*)menu1, item2);

#GSList *list = g_slist_alloc();

name_menu_bar = "menubar"
name_menu_item = "item"
name_menu_menu = "menu"

prespace1 = "	"
prespace2 = "	"

radios = dict()

def print_item(label, path, has_subs, part):
	fill1 = 26 - 2*(len(path) - 1) - len(name_menu_item)
	fill2 = 26 - 2*(len(path) - 1) - len(name_menu_menu)
	fill3 = 18 - 2*(len(path) - 1) - len(name_menu_item)
	fill4 = 18 - 2*(len(path) - 1) - len(name_menu_menu)
	fill5 = 18 - 2*(len(path) - 1) - len(name_menu_bar)
	fill6 = 30 - len(label[1])
	name_item = name_menu_item 
	name_menu = name_menu_menu
	for p in path:
		name_item += "_"+ str(p)
		name_menu += "_"+ str(p)
	if part == 1:
		text_item = prespace1 + "GtkWidget *" + name_item
		text_menu = prespace1 + "GtkWidget *" + name_menu
		text_item += fill1 * ' '
		text_menu += fill2 * ' '
		if len(label) <= 2:
			text_item += "= gtk_menu_item_new_with_mnemonic(\"" + label[0] + "\");"
		elif label[2] == "check":
			text_item += "= gtk_check_menu_item_new_with_mnemonic(\"" + label[0] + "\");"
		elif label[2] == "radio":
			group = label[3]
			if radios.keys().count(group) == 0:
				text_item += "= gtk_radio_menu_item_new_with_mnemonic(NULL, \"" + label[0] + "\");"
				radios[group] = name_item
			else:
				text_item += "= gtk_radio_menu_item_new_with_mnemonic(gtk_radio_menu_item_get_group((GtkRadioMenuItem*)" + radios[group] +  "), \"" + label[0] + "\");"
		else:
			text_item += "= gtk_menu_item_new_with_mnemonic(\"" + label[0] + "\");"
		text_menu += "= gtk_menu_new();"
		print text_item
		if has_subs:
			print text_menu
	if part == 2:
		if has_subs:
			print prespace2 + "gtk_menu_item_set_submenu(GTK_MENU_ITEM(" + name_item + ")," + fill3 * ' ' + name_menu + ");"
	if part == 3:
		fill = 0
		if len(path) == 1:
			parent_menu = name_menu_bar 
			fill = fill5
		else:
			parent_menu = name_menu[:-2]
			fill = fill4
		print prespace2 + "gtk_menu_shell_append(GTK_MENU_SHELL("+ parent_menu + "), " + fill * ' ' + name_item +");" 
	if part == 4:
		if label[1] != '':
			print prespace2 + "menubar -> " + label[1] + fill6 * ' '  + " = " + name_item + ";"
	if part == 5:
		if label[1] != '':
			print prespace2 + "GtkWidget * " + label[1] + ";"

def print_rek(structure, path, part):
	if path != []:
		print_item(structure[0], path, len(structure) > 1, part)
	if len(structure) > 1:
		for index in range(1,len(structure)):
			print_rek(structure[index], path + [index], part)

def print_out(structure, mode):
	#print
	#print "GtkWidget *" + name_menu_bar + " = gtk_menu_bar_new();"
	if mode == "2":
		print_rek(structure, [], 1)
		print_rek(structure, [], 2)
		print_rek(structure, [], 3)
		print_rek(structure, [], 4)
	if mode == "1":
		print_rek(structure, [], 5)

def count_blanks(line):
	count = 0
	for c in line:
		if c == ' ':
			count += 1
		else:
			break
	return count

if len(sys.argv) != 3:
	print "usage: menu_gen.py <mode> <menu_file>"
	print "mode: 1 = global vars; 2 = rest"
else:
	structure = ["root"];
	active = structure;
	path = []

	mode = sys.argv[1]
	filename = sys.argv[2]
	#print "processing file:", filename
	file = open(filename)
	lines = file.readlines()

	ilevel = 0

	for line in lines:
		level = count_blanks(line)
		options = line.split(';')
		for i in range(len(options)):
			options[i] = string.strip(options[i])
		if level - ilevel == 0:
			active.append([options])
		elif level - ilevel == 1:
			path.append(len(active) - 1)
			ilevel = level
			active = active[-1]
			active.append([options])
		elif level - ilevel < 0:
			times = ilevel - level
			while (times > 0):
				path.pop()
				times -= 1
			ilevel += level - ilevel
			active = structure
			for pos in path:
				active = active[pos]
			active.append([options])

	#print structure
	print_out(structure, mode)
