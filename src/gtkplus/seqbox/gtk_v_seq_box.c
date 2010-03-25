/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

#include "gtk_v_seq_box.h"

G_DEFINE_TYPE (GtkVSeqBox, gtk_v_seq_box, GTK_TYPE_SEQ_BOX)

static void
gtk_v_seq_box_class_init (GtkVSeqBoxClass *class)
{
}

static void
gtk_v_seq_box_init (GtkVSeqBox *v_seq_box)
{
  gtk_orientable_set_orientation (GTK_ORIENTABLE (v_seq_box),
                                  GTK_ORIENTATION_VERTICAL);

  _gtk_seq_box_set_old_defaults (GTK_SEQ_BOX (v_seq_box));
}

GtkWidget * gtk_v_seq_box_new	(gboolean homogeneous,
				 gint     spacing,
				 GCompareDataFunc cmp_func,
				 gpointer cmp_data,
				 GDestroyNotify key_destroy)
{
  GtkWidget * box = g_object_new (GTK_TYPE_V_SEQ_BOX,
                       "spacing",     spacing,
                       "homogeneous", homogeneous ? TRUE : FALSE,
                       NULL);
  gtk_seq_box_set_compare_function(GTK_SEQ_BOX(box), cmp_func);
  gtk_seq_box_set_compare_data(GTK_SEQ_BOX(box), cmp_data);
  gtk_seq_box_set_destroy_function(GTK_SEQ_BOX(box), key_destroy);
  return box;
}

#define __GTK_V_SEQ_BOX_C__
