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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
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

#if defined(GTK_DISABLE_SINGLE_INCLUDES) && !defined (__GTK_H_INSIDE__) && !defined (GTK_COMPILATION)
#error "Only <gtk/gtk.h> can be included directly."
#endif

#ifndef __GTK_SEQ_BOX_H__
#define __GTK_SEQ_BOX_H__


#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>


G_BEGIN_DECLS


#define GTK_TYPE_SEQ_BOX            (gtk_seq_box_get_type ())
#define GTK_SEQ_BOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SEQ_BOX, GtkSeqBox))
#define GTK_SEQ_BOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_SEQ_BOX, GtkSeqBoxClass))
#define GTK_IS_SEQ_BOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SEQ_BOX))
#define GTK_IS_SEQ_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SEQ_BOX))
#define GTK_SEQ_BOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_SEQ_BOX, GtkSeqBoxClass))


typedef struct _GtkSeqBox	      GtkSeqBox;
typedef struct _GtkSeqBoxClass   GtkSeqBoxClass;
typedef struct _GtkSeqBoxChild   GtkSeqBoxChild;

struct _GtkSeqBox
{
  GtkContainer container;

  /*< public >*/
  GSequence *GSEAL (children);
  GSequence *GSEAL (children_inverse);
  gint16 GSEAL (spacing);
  guint GSEAL (homogeneous) : 1;

  GCompareDataFunc cmp_func;
  gpointer cmp_data;
  GDestroyNotify key_destroy;
};

struct _GtkSeqBoxClass
{
  GtkContainerClass parent_class;
};

struct _GtkSeqBoxChild
{
  GtkWidget *widget;
  gpointer key;
  guint16 padding;
  guint expand : 1;
  guint fill : 1;
  guint pack : 1;
  guint is_secondary : 1;
};


GType       gtk_seq_box_get_type            (void) G_GNUC_CONST;
GtkWidget* _gtk_seq_box_new                 (GtkOrientation  orientation,
                                             gboolean        homogeneous,
                                             gint            spacing);

void gtk_seq_box_insert	(GtkSeqBox * box,
			 gpointer key,
			 GtkWidget * child,
			 gboolean expand,
			 gboolean fill,
			 guint padding);

void gtk_seq_box_reorder (GtkSeqBox * box,
			  gpointer key_old,
			  gpointer key_new);

void gtk_seq_remove	(GtkSeqBox * box,
			 gpointer key);

GtkWidget * gtk_seq_box_get	(GtkSeqBox * box,
			 	 gpointer key);

gpointer gtk_seq_box_get_key	(GtkSeqBox * box,
				 GtkWidget * child);


void gtk_seq_box_set_compare_function	(GtkSeqBox * box,
					 GCompareDataFunc cmp_func);

void gtk_seq_box_set_compare_data	(GtkSeqBox * box,
					 gpointer cmp_data);

void gtk_seq_box_set_destroy_function	(GtkSeqBox * box,
					 GDestroyNotify key_destroy);

void        gtk_seq_box_pack_start          (GtkSeqBox         *box,
                                         GtkWidget      *child,
                                         gboolean        expand,
                                         gboolean        fill,
                                         guint           padding);
void        gtk_seq_box_pack_end            (GtkSeqBox         *box,
                                         GtkWidget      *child,
                                         gboolean        expand,
                                         gboolean        fill,
                                         guint           padding);

#ifndef GTK_DISABLE_DEPRECATED
void        gtk_seq_box_pack_start_defaults (GtkSeqBox         *box,
                                         GtkWidget      *widget);
void        gtk_seq_box_pack_end_defaults   (GtkSeqBox         *box,
                                         GtkWidget      *widget);
#endif

void        gtk_seq_box_set_homogeneous     (GtkSeqBox         *box,
                                         gboolean        homogeneous);
gboolean    gtk_seq_box_get_homogeneous     (GtkSeqBox         *box);
void        gtk_seq_box_set_spacing         (GtkSeqBox         *box,
                                         gint            spacing);
gint        gtk_seq_box_get_spacing         (GtkSeqBox         *box);

void        gtk_seq_box_reorder_child       (GtkSeqBox         *box,
                                         GtkWidget      *child,
                                         gint            position);

void        gtk_seq_box_query_child_packing (GtkSeqBox         *box,
                                         GtkWidget      *child,
                                         gboolean       *expand,
                                         gboolean       *fill,
                                         guint          *padding,
                                         GtkPackType    *pack_type);
void        gtk_seq_box_set_child_packing   (GtkSeqBox         *box,
                                         GtkWidget      *child,
                                         gboolean        expand,
                                         gboolean        fill,
                                         guint           padding,
                                         GtkPackType     pack_type);

/* internal API */
void        _gtk_seq_box_set_old_defaults   (GtkSeqBox         *box);
gboolean    _gtk_seq_box_get_spacing_set    (GtkSeqBox         *box);
void        _gtk_seq_box_set_spacing_set    (GtkSeqBox         *box,
                                         gboolean        spacing_set);

G_END_DECLS

#endif /* __GTK_SEQ_BOX_H__ */
