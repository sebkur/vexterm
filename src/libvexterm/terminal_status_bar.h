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

#ifndef _TERMINAL_STATUS_BAR_H_
#define _TERMINAL_STATUS_BAR_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "terminal_widget.h"

#define LIBVEX_TYPE_TERMINAL_STATUS_BAR           (terminal_status_bar_get_type ())
#define LIBVEX_TERMINAL_STATUS_BAR(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIBVEX_TYPE_TERMINAL_STATUS_BAR, TerminalStatusBar))
#define LIBVEX_TERMINAL_STATUS_BAR_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), LIBVEX_TYPE_TERMINAL_STATUS_BAR, TerminalStatusBarClass))
#define LIBVEX_IS_TERMINAL_STATUS_BAR(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIBVEX_TYPE_TERMINAL_STATUS_BAR))
#define LIBVEX_IS_TERMINAL_STATUS_BAR_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), LIBVEX_TYPE_TERMINAL_STATUS_BAR))
#define LIBVEX_TERMINAL_STATUS_BAR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), LIBVEX_TYPE_TERMINAL_STATUS_BAR, TerminalStatusBarClass))

GType   terminal_status_bar_get_type (void) G_GNUC_CONST;

typedef struct _TerminalStatusBar        TerminalStatusBar;
typedef struct _TerminalStatusBarClass   TerminalStatusBarClass;

struct _TerminalStatusBar
{
	GtkHBox parent;

	TerminalWidget * terminal_widget;

	GtkWidget * screen_label;
	GtkWidget * keypad_label;
	GtkWidget * size_label;
};

struct _TerminalStatusBarClass
{
	GtkHBoxClass parent_class;

	//void (* function_name) (TerminalStatusBar *terminal_status_bar);
};

GType terminal_status_bar_get_type (void) G_GNUC_CONST;

GtkWidget * terminal_status_bar_new(TerminalWidget * terminal_widget);

#endif /* _TERMINAL_STATUS_BAR_H_ */
