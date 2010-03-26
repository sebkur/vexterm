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

//TODO: connect to profile-added and friends to keep profiles-menu up to date
// also, when config is reset (due cancel in preferences-dialog), reconstruct menu

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include "../helpers/tool.h"
#include "../helpers/util.h"

#include "../libvexterm/terminal_config.h"
#include "vex_term.h"
#include "vex_single.h"
#include "vex_single_container.h"
#include "vex_paths.h"
#include "config/vex_profile.h"
#include "config/vex_config_writer.h"
#include "about/about_widget.h"
#include "preferences/preferences.h"

#define DEFAULT_PROFILE "default"

G_DEFINE_TYPE (VexTerm, vex_term, GTK_TYPE_VBOX);

/*enum
{
        SIGNAL_NAME_1,
        SIGNAL_NAME_n,
        LAST_SIGNAL
};*/

//static guint vex_term_signals[LAST_SIGNAL] = { 0 };
//g_signal_emit (widget, vex_term_signals[SIGNAL_NAME_n], 0);

void vex_term_constructor(VexTerm * vex_term, VexLayeredConfig * vlc);
void vex_term_add_tab(VexTerm * vex_term);
void vex_term_close_tab(VexTerm * vex_term);
void vex_term_move_tab_focus(VexTerm * vex_term, int direction);
void vex_term_move_tab(VexTerm * vex_term, int direction);

static void vex_term_notebook_focus_in_cb(GtkWidget * widget, GdkEventFocus * event, VexTerm * vex_term);
static void vex_term_notebook_switch_page_cb(GtkNotebook * nb, GtkNotebookPage * page, int pnum, VexTerm * vex_term);
static gboolean vex_term_key_press_cb(GtkWidget * widget, GdkEventKey * event, VexTerm * vex_term);
static void vex_term_exited_cb(VexSingle * vex_single, VexTerm * vex_term);
static gboolean vex_term_close_button_clicked_cb(GtkWidget * widget, VexTerm * vex_term);
void vex_term_kill_terminal(VexSingle * vex);

static void menubar_scrolling_region_cb(GtkWidget * widget, VexTerm * vex_term);
static void menubar_status_bar_cb(GtkWidget * widget, VexTerm * vex_term);
static gboolean show_manual_cb(GtkWidget *widget, VexTerm * vex_term);
static gboolean show_about_vexterm_cb(GtkWidget *widget, VexTerm * vex_term);
static gboolean show_about_license_cb(GtkWidget *widget, VexTerm * vex_term);
static gboolean show_preferences_cb(GtkWidget *widget, VexTerm * vex_term);
static gboolean menubar_new_window_cb(GtkWidget *widget, VexTerm * vex_term);
static gboolean menubar_new_tab_cb(GtkWidget *widget, VexTerm * vex_term);

void vex_term_add_profile_entry(VexTerm * vex_term, char * name);
void vex_term_remove_profile_entry(VexTerm * vex_term, char * name);
void vex_term_rename_profile_entry(VexTerm * vex_term, char * name, char * new_name);

void vex_term_toggle_fullscreen(VexTerm * vex_term);
void vex_term_toggle_menu(VexTerm * vex_term);

GtkWidget * vex_term_new(VexLayeredConfig * vlc)
{
	VexTerm * vex_term = g_object_new(VEX_TYPE_VEX_TERM, NULL);

	vex_term_constructor(vex_term, vlc);

	return GTK_WIDGET(vex_term);
}

static void vex_term_class_init(VexTermClass *class)
{
        /*vex_term_signals[SIGNAL_NAME_n] = g_signal_new(
                "signal-name-n",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (VexTermClass, function_name),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);*/
}

static void vex_term_init(VexTerm *vex_term)
{
}

void vex_term_constructor(VexTerm * vex_term, VexLayeredConfig * vlc)
{
	vex_term -> vlc = vlc;
	vex_term -> notebook = gtk_notebook_new();
	vex_term -> current_index = 0;
	vex_term -> menu = VEX_MENU(menu_new());
	vex_term -> menu_visible = TRUE;
	vex_term -> show_status_bar = vex_config_get_show_status_bar(vex_layered_config_get_config_local(vlc));
	vex_term -> show_scrolling_region = vex_config_get_show_scrolling_region(vex_layered_config_get_config_local(vlc));
	vex_term -> preferences = NULL;

	g_object_set(vex_term -> notebook, "tab-border", 0, NULL);
	g_object_set(vex_term -> notebook, "tab-vborder", 0, NULL);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(vex_term -> notebook), TRUE);
	// TODO: wait for a version of gtk that supports this
//	GtkWidget * button_tab = gtk_button_new_with_label("ct");
//	gtk_notebook_set_action_widget(GTK_NOTEBOOK(vex_term -> notebook),
//		button_tab, GTK_PACK_START);

	gtk_box_pack_start(GTK_BOX(vex_term), GTK_WIDGET(vex_term -> menu), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vex_term), vex_term -> notebook, TRUE, TRUE, 0);

	vex_term_add_tab(vex_term);

	VexSingleContainer * vcs = VEX_VEX_SINGLE_CONTAINER(gtk_notebook_get_nth_page(GTK_NOTEBOOK(vex_term -> notebook), 0));
	gtk_menu_item_set_submenu(
		GTK_MENU_ITEM(vex_term -> menu -> menu_profiles), vcs -> menu_profiles);

	g_signal_connect(
		G_OBJECT(vex_term -> notebook), "focus-in-event",
		G_CALLBACK(vex_term_notebook_focus_in_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex_term -> notebook), "switch-page",
		G_CALLBACK(vex_term_notebook_switch_page_cb), vex_term);

	/* set menu item status according to config */
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(vex_term -> menu -> menu_show_status_bar),
		vex_term -> show_status_bar);
	gtk_check_menu_item_set_active(
		GTK_CHECK_MENU_ITEM(vex_term -> menu -> menu_show_scrolling_region),
		vex_term -> show_scrolling_region);

	/* connect to the menu items */
	g_signal_connect(
		G_OBJECT(vex_term -> menu -> menu_show_scrolling_region), 	"toggled", 
		G_CALLBACK(menubar_scrolling_region_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex_term -> menu -> menu_show_status_bar), 		"toggled", 
		G_CALLBACK(menubar_status_bar_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex_term -> menu -> menu_help_manual), 		"activate", 
		G_CALLBACK(show_manual_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex_term -> menu -> menu_help_about_vexterm), 		"activate", 
		G_CALLBACK(show_about_vexterm_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex_term -> menu -> menu_help_license),		"activate", 
		G_CALLBACK(show_about_license_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex_term -> menu -> menu_options_preferences),		"activate", 
		G_CALLBACK(show_preferences_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex_term -> menu -> menu_file_new_window),		"activate", 
		G_CALLBACK(menubar_new_window_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex_term -> menu -> menu_file_new_tab),		"activate", 
		G_CALLBACK(menubar_new_tab_cb), vex_term);

	gtk_widget_set_sensitive(vex_term -> menu -> menu_file_new_window, FALSE);
}

void vex_term_add_tab(VexTerm * vex_term)
{
	char * profile_name = vex_config_get_active_profile(vex_layered_config_get_config_local(vex_term -> vlc));
	if (profile_name == NULL){
		profile_name = vex_config_get_active_profile(vex_layered_config_get_config_global(vex_term -> vlc));
	}
	if (profile_name == NULL){
		profile_name = DEFAULT_PROFILE;
	}
	printf("ADD TAB: profile name: %s\n", profile_name);
	VexProfile * profile = vex_layered_config_get_profile_by_name(vex_term -> vlc, profile_name);
	printf("ADD TAB: profile ptr: %p\n", profile);

	GtkNotebook * nb = GTK_NOTEBOOK(vex_term -> notebook);

	/* retrieve the currently activated terminal's pwd so that we can start
	 * the new one at the same path */
	char * pwd = NULL;
	int x = gtk_notebook_get_current_page(nb);
	if (x >= 0){
		VexSingleContainer * vcs = VEX_VEX_SINGLE_CONTAINER(gtk_notebook_get_nth_page(nb, x));
		VexSingle * vex_current = vex_single_container_get_vex_single(vcs);
		pwd = vex_single_get_pwd(vex_current);
	}

	/* set up a new terminal */
	VexSingleContainer * vcs = VEX_VEX_SINGLE_CONTAINER(vex_single_container_new(vex_term -> vlc, profile_name, pwd));

	VexSingle * vex = VEX_VEX_SINGLE(vex_single_container_get_vex_single(vcs));
	vex_single_set_show_status_bar(vex, vex_term -> show_status_bar);
	vex_single_set_show_scrolling_region(vex, vex_term -> show_scrolling_region);

	/* add it to the notebook */
	GtkWidget * label = gtk_hbox_new(FALSE, 0);
	GtkWidget * label_label = gtk_label_new("term");

	GtkWidget * label_button = gtk_button_new();
	GtkWidget * image_close = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	gtk_container_add(GTK_CONTAINER(label_button), image_close);
	gtk_button_set_relief(GTK_BUTTON(label_button), GTK_RELIEF_NONE);
	gtk_button_set_focus_on_click(GTK_BUTTON(label_button), FALSE);
	int button_width, button_height;
	gtk_icon_size_lookup_for_settings(gtk_widget_get_settings(GTK_WIDGET(vex_term)),
		GTK_ICON_SIZE_MENU, &button_width, &button_height);
	//TODO: this is not nice
	gtk_widget_set_size_request(label_button, button_width + 4, button_height + 4);

	GtkWidget * label_vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_end(GTK_BOX(label_vbox), label_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(label), label_label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(label), label_vbox, FALSE, FALSE, 0);
	gtk_widget_show_all(label);

	int p = gtk_notebook_append_page(nb, GTK_WIDGET(vcs), label);
	gtk_notebook_set_tab_label_packing(nb,
		GTK_WIDGET(vcs), TRUE, TRUE, GTK_PACK_START);

	g_object_set(G_OBJECT(label_button), "user-data", vex, NULL);
	/* connect some events */
	g_signal_connect(
		G_OBJECT(label_button), "clicked",
		G_CALLBACK(vex_term_close_button_clicked_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex -> terminal_widget), "key-press-event",
		G_CALLBACK(vex_term_key_press_cb), vex_term);
	g_signal_connect(
		G_OBJECT(vex), "exited",
		G_CALLBACK(vex_term_exited_cb), vex_term);

	/* and show it */
	gtk_widget_show_all(GTK_WIDGET(vcs));
	gtk_notebook_set_current_page(nb, p);
}

void vex_term_kill_terminal(VexSingle * vex)
{
	terminal_stop(vex -> terminal);
}

static gboolean vex_term_close_button_clicked_cb(GtkWidget * widget, VexTerm * vex_term)
{
	VexSingle * vex;
	g_object_get(G_OBJECT(widget), "user-data", &vex, NULL);
	vex_term_kill_terminal(vex);
	return FALSE;
}

void vex_term_close_tab(VexTerm * vex_term)
{
	GtkNotebook * nb = GTK_NOTEBOOK(vex_term -> notebook);
	int x = gtk_notebook_get_current_page(nb);
	if (x >= 0){
		VexSingleContainer * vcs = VEX_VEX_SINGLE_CONTAINER(gtk_notebook_get_nth_page(nb, x));
		VexSingle * vex_current = vex_single_container_get_vex_single(vcs);
		vex_term_kill_terminal(vex_current);
	}
}

static void vex_term_exited_cb(VexSingle * vex_single, VexTerm * vex_term)
{
	GtkNotebook * nb = GTK_NOTEBOOK(vex_term -> notebook);
	GtkWidget * vcs = gtk_widget_get_parent(GTK_WIDGET(vex_single));
	int x = gtk_notebook_page_num(nb, vcs);
	if (x < 0) return;

	/* remove from notebook */
	gtk_notebook_remove_page(nb, x);
	/* exit if this was the last terminal */
	int n = gtk_notebook_get_n_pages(nb);
	if (n == 0) gtk_main_quit();
}

static void vex_term_notebook_focus_in_cb(GtkWidget * widget, GdkEventFocus * event, VexTerm * vex_term)
{
	GtkNotebook * nb = GTK_NOTEBOOK(vex_term -> notebook);
	gtk_widget_grab_focus(gtk_notebook_get_nth_page(nb, gtk_notebook_get_current_page(nb)));
}

void vex_term_move_tab_focus(VexTerm * vex_term, int direction)
{
	GtkNotebook * nb = GTK_NOTEBOOK(vex_term -> notebook);
	int x = gtk_notebook_get_current_page(nb);
	int n = gtk_notebook_get_n_pages(nb);
	int a = x;
	if (direction == 0 && x > 0){
		a -= 1;
	}
	if (direction == 1 && x < n - 1){
		a += 1;
	}
	gtk_notebook_set_current_page(nb, a);
}

void vex_term_move_tab(VexTerm * vex_term, int direction)
{
	GtkNotebook * nb = GTK_NOTEBOOK(vex_term -> notebook);
	int x = gtk_notebook_get_current_page(nb);
	int n = gtk_notebook_get_n_pages(nb);
	int a = x;
	if (direction == 0 && x > 0){
		a -= 1;
	}
	if (direction == 1 && x < n - 1){
		a += 1;
	}
	GtkWidget * widget = gtk_notebook_get_nth_page(nb, x);
	gtk_notebook_reorder_child(nb, widget, a);
}

static gboolean vex_term_key_press_cb(GtkWidget * widget, GdkEventKey * event, VexTerm * vex_term)
{
	guint modifiers = gtk_accelerator_get_default_mod_mask();
	if ((event -> state & modifiers) == (GDK_CONTROL_MASK)){
		switch(event -> keyval){
			case GDK_Page_Up:{
				vex_term_move_tab_focus(vex_term, 0);
				return TRUE;
			}
			case GDK_Page_Down:{
				vex_term_move_tab_focus(vex_term, 1);
				return TRUE;
			}
		}
	}
	if ((event -> state & modifiers) == (GDK_CONTROL_MASK | GDK_SHIFT_MASK)){
		switch(event -> keyval){
			case GDK_T:{
				vex_term_add_tab(vex_term);
				break;
			}
			case GDK_W:{
				vex_term_close_tab(vex_term);
				break;
			}
			case GDK_Page_Up:{
				vex_term_move_tab(vex_term, 0);
				return TRUE;
			}
			case GDK_Page_Down:{
				vex_term_move_tab(vex_term, 1);
				return TRUE;
			}
			case GDK_F10:{
				vex_term_toggle_menu(vex_term);
				return TRUE;
			}
			case GDK_F11:{
				vex_term_toggle_fullscreen(vex_term);
				return TRUE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

static void menubar_status_bar_cb(GtkWidget * widget, VexTerm * vex_term)
{
	gboolean show = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
	vex_term -> show_status_bar = show;

	/* iterate all terminals */
	GtkNotebook * nb = GTK_NOTEBOOK(vex_term -> notebook);
	int t;
	for (t = 0; t < gtk_notebook_get_n_pages(nb); t++){
		VexSingleContainer * vcs = VEX_VEX_SINGLE_CONTAINER(gtk_notebook_get_nth_page(nb, t));
		/* trigger to show it */
		vex_single_set_show_status_bar(vcs -> vex_single, show);
	}
}

static void menubar_scrolling_region_cb(GtkWidget * widget, VexTerm * vex_term)
{
	gboolean show = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget));
	vex_term -> show_scrolling_region = show;

	/* iterate all terminals */
	GtkNotebook * nb = GTK_NOTEBOOK(vex_term -> notebook);
	int t;
	for (t = 0; t < gtk_notebook_get_n_pages(nb); t++){
		VexSingleContainer * vcs = VEX_VEX_SINGLE_CONTAINER(gtk_notebook_get_nth_page(nb, t));
		/* trigger to show it */
		vex_single_set_show_scrolling_region(vcs -> vex_single, show);
	}
}

static gboolean menubar_new_window_cb(GtkWidget *widget, VexTerm * vex_term)
{
	return FALSE;
}

static gboolean menubar_new_tab_cb(GtkWidget *widget, VexTerm * vex_term)
{
	vex_term_add_tab(vex_term);
	return FALSE;
}

/****************************************************************************************************
* dialogs
****************************************************************************************************/
void show_about(VexTerm * vex_term, int preselect)
{
	/***************************************************************************
	 * window setup ************************************************************
	 ***************************************************************************/
	int width = 512, height = 384;
	GtkWidget * dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(dialog), "About");
	gtk_window_set_default_size(GTK_WINDOW(dialog), width, height);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	GtkWidget * window = find_containing_gtk_window(GTK_WIDGET(vex_term));
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	//g_signal_connect(G_OBJECT(dialog), "hide", G_CALLBACK(exit), NULL);

	/***************************************************************************
	 * window content **********************************************************
	 ***************************************************************************/
	GtkWidget * about = about_widget_new(preselect);

	GtkWidget * box_h = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box_h), GTK_WIDGET(about), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(dialog), box_h);

	gtk_widget_show_all(dialog);
}

static gboolean show_manual_cb(GtkWidget *widget, VexTerm * vex_term)
{
	show_about(vex_term, 0);
	return FALSE;
}

static gboolean show_about_vexterm_cb(GtkWidget *widget, VexTerm * vex_term)
{
	show_about(vex_term, 1);
	return FALSE;
}

static gboolean show_about_license_cb(GtkWidget *widget, VexTerm * vex_term)
{
	show_about(vex_term, 2);
	return FALSE;
}

static void preferences_response_cb(GtkDialog * dialog, int response, VexTerm * vex_term)
{
	if (response == GTK_RESPONSE_OK){
		printf("save config\n");

		char * conf_file_local = vex_term_get_config_file_local();
		char * conf_file_local_tmp = g_strconcat(conf_file_local, ".tmp", NULL);

		printf("writing to: %s\n", conf_file_local_tmp);
		VexConfigWriter * vcw = vex_config_writer_new();
		vex_config_writer_write_config_to_file(vcw, conf_file_local_tmp, 
			vex_layered_config_get_config_local(vex_term -> vlc));

		printf("moving to: %s\n", conf_file_local);
		//if (success)
		rename(conf_file_local_tmp, conf_file_local);

		//TODO: we have to release the copied version of vlc
	}else{
		printf("abandon config\n");
		GtkNotebook * nb = GTK_NOTEBOOK(vex_term -> notebook);
		int t;
		for (t = 0; t < gtk_notebook_get_n_pages(nb); t++){
			VexSingleContainer * vcs = 
				VEX_VEX_SINGLE_CONTAINER(gtk_notebook_get_nth_page(nb, t));
			vex_single_container_set_config(vcs, vex_term -> vlc_copy);
		}
		vex_term -> vlc = vex_term -> vlc_copy;
	}
	vex_term -> vlc_copy = NULL; // free copied config
	vex_term -> preferences = NULL;
	gtk_widget_hide(GTK_WIDGET(dialog));
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static gboolean show_preferences_cb(GtkWidget *widget, VexTerm * vex_term)
{
	if (vex_term -> preferences == NULL){
		VexConfig * config_global = vex_layered_config_get_config_global(vex_term -> vlc);
		VexConfig * config_local = vex_layered_config_get_config_local(vex_term -> vlc);

		vex_term -> vlc_copy = vex_layered_config_new(NULL, NULL);
		VexConfig * config_local_copy = vex_layered_config_get_config_local(vex_term -> vlc_copy);
		VexConfig * config_global_copy = vex_layered_config_get_config_global(vex_term -> vlc_copy);
		vex_config_deepcopy(config_local, config_local_copy);
		vex_config_deepcopy(config_global, config_global_copy);

		Preferences * d = VEX_PREFERENCES(preferences_new(vex_term -> vlc));

		gtk_window_set_title(GTK_WINDOW(d), "VexTerm Preferences");
		gtk_window_set_default_size(GTK_WINDOW(d), 500, 400);
		gtk_window_set_position(GTK_WINDOW(d), GTK_WIN_POS_CENTER);
		//gtk_window_set_transient_for(GTK_WINDOW(d), NULL);

		gtk_widget_show_all(GTK_WIDGET(d));

		vex_term -> preferences = GTK_WIDGET(d);

		g_signal_connect(
			G_OBJECT(d), "response",
			G_CALLBACK(preferences_response_cb), vex_term);
	}else{
		gtk_window_present(GTK_WINDOW(vex_term -> preferences));
	}

	return FALSE;
}

static void vex_term_notebook_switch_page_cb(GtkNotebook * nb, GtkNotebookPage * page, int pnum, VexTerm * vex_term)
{
	printf("pnum %d\n", pnum);
	vex_term -> current_index = pnum;

	VexSingleContainer * vcs = VEX_VEX_SINGLE_CONTAINER(gtk_notebook_get_nth_page(nb, pnum));
	gtk_menu_item_set_submenu(
		GTK_MENU_ITEM(vex_term -> menu -> menu_profiles), vcs -> menu_profiles);
}

void vex_term_toggle_fullscreen(VexTerm * vex_term)
{
	GtkWidget * widget = find_containing_gtk_window(GTK_WIDGET(vex_term));
	GdkWindow * window = widget -> window;
	gboolean fullscreened = (gdk_window_get_state(window) & GDK_WINDOW_STATE_FULLSCREEN) != 0;
	if (fullscreened){
		gdk_window_unfullscreen(window);
	}else{
		gdk_window_fullscreen(window);
	}
}

void vex_term_toggle_menu(VexTerm * vex_term)
{
	vex_term -> menu_visible = !vex_term -> menu_visible;
	if (vex_term -> menu_visible){
		gtk_widget_show(GTK_WIDGET(vex_term -> menu));
	}else{
		gtk_widget_hide(GTK_WIDGET(vex_term -> menu));
	}
}

