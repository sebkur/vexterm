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

#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkprivate.h>
#include <gdk/gdk.h>

#include "gtk_seq_box.h"
#include "../../helpers/util.h"

#define P_(String) (String)

enum {
  PROP_0,
  PROP_ORIENTATION,
  PROP_SPACING,
  PROP_HOMOGENEOUS
};

enum {
  CHILD_PROP_0,
  CHILD_PROP_EXPAND,
  CHILD_PROP_FILL,
  CHILD_PROP_PADDING,
  CHILD_PROP_PACK_TYPE,
  CHILD_PROP_POSITION
};


typedef struct _GtkSeqBoxPrivate GtkSeqBoxPrivate;

struct _GtkSeqBoxPrivate
{
  GtkOrientation orientation;
  guint          default_expand : 1;
  guint          spacing_set    : 1;
};

typedef struct{
	GtkWidget * child;
	gpointer key;
}InverseEntry;

#define GTK_SEQ_BOX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_SEQ_BOX, GtkSeqBoxPrivate))


static void gtk_seq_box_set_property       (GObject        *object,
                                        guint           prop_id,
                                        const GValue   *value,
                                        GParamSpec     *pspec);
static void gtk_seq_box_get_property       (GObject        *object,
                                        guint           prop_id,
                                        GValue         *value,
                                        GParamSpec     *pspec);

static void gtk_seq_box_size_request       (GtkWidget      *widget,
                                        GtkRequisition *requisition);
static void gtk_seq_box_size_allocate      (GtkWidget      *widget,
                                        GtkAllocation  *allocation);

static void gtk_seq_box_add                (GtkContainer   *container,
                                        GtkWidget      *widget);
static void gtk_seq_box_remove             (GtkContainer   *container,
                                        GtkWidget      *widget);
static void gtk_seq_box_forall             (GtkContainer   *container,
                                        gboolean        include_internals,
                                        GtkCallback     callback,
                                        gpointer        callback_data);
static void gtk_seq_box_set_child_property (GtkContainer   *container,
                                        GtkWidget      *child,
                                        guint           property_id,
                                        const GValue   *value,
                                        GParamSpec     *pspec);
static void gtk_seq_box_get_child_property (GtkContainer   *container,
                                        GtkWidget      *child,
                                        guint           property_id,
                                        GValue         *value,
                                        GParamSpec     *pspec);
static GType gtk_seq_box_child_type        (GtkContainer   *container);

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (GtkSeqBox, gtk_seq_box, GTK_TYPE_CONTAINER,
                                  G_IMPLEMENT_INTERFACE (GTK_TYPE_ORIENTABLE,
                                                         NULL));

static void
gtk_seq_box_class_init (GtkSeqBoxClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (class);

  object_class->set_property = gtk_seq_box_set_property;
  object_class->get_property = gtk_seq_box_get_property;

  widget_class->size_request = gtk_seq_box_size_request;
  widget_class->size_allocate = gtk_seq_box_size_allocate;

  container_class->add = gtk_seq_box_add;
  container_class->remove = gtk_seq_box_remove;
  container_class->forall = gtk_seq_box_forall;
  container_class->child_type = gtk_seq_box_child_type;
  container_class->set_child_property = gtk_seq_box_set_child_property;
  container_class->get_child_property = gtk_seq_box_get_child_property;

  g_object_class_override_property (object_class,
                                    PROP_ORIENTATION,
                                    "orientation");

  g_object_class_install_property (object_class,
                                   PROP_SPACING,
                                   g_param_spec_int ("spacing",
                                                     P_("Spacing"),
                                                     P_("The amount of space between children"),
                                                     0,
                                                     G_MAXINT,
                                                     0,
                                                     GTK_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_HOMOGENEOUS,
                                   g_param_spec_boolean ("homogeneous",
							 P_("Homogeneous"),
							 P_("Whether the children should all be the same size"),
							 FALSE,
							 GTK_PARAM_READWRITE));

  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_EXPAND,
					      g_param_spec_boolean ("expand", 
								    P_("Expand"), 
								    P_("Whether the child should receive extra space when the parent grows"),
								    TRUE,
								    GTK_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_FILL,
					      g_param_spec_boolean ("fill", 
								    P_("Fill"), 
								    P_("Whether extra space given to the child should be allocated to the child or used as padding"),
								    TRUE,
								    GTK_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_PADDING,
					      g_param_spec_uint ("padding", 
								 P_("Padding"), 
								 P_("Extra space to put between the child and its neighbors, in pixels"),
								 0, G_MAXINT, 0,
								 GTK_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_PACK_TYPE,
					      g_param_spec_enum ("pack-type", 
								 P_("Pack type"), 
								 P_("A GtkPackType indicating whether the child is packed with reference to the start or end of the parent"),
								 GTK_TYPE_PACK_TYPE, GTK_PACK_START,
								 GTK_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_POSITION,
					      g_param_spec_int ("position", 
								P_("Position"), 
								P_("The index of the child in the parent"),
								-1, G_MAXINT, 0,
								GTK_PARAM_READWRITE));

  g_type_class_add_private (object_class, sizeof (GtkSeqBoxPrivate));
}

static void
gtk_seq_box_init (GtkSeqBox *box)
{
  GtkSeqBoxPrivate *private = GTK_SEQ_BOX_GET_PRIVATE (box);

  GTK_WIDGET_SET_FLAGS (box, GTK_NO_WINDOW);
  gtk_widget_set_redraw_on_allocate (GTK_WIDGET (box), FALSE);

  box->children = g_sequence_new(NULL);
  box->children_inverse = g_sequence_new(NULL);
  box->spacing = 0;
  box->homogeneous = FALSE;

  private->orientation = GTK_ORIENTATION_HORIZONTAL;
  private->default_expand = FALSE;
  private->spacing_set = FALSE;
}

static void
gtk_seq_box_set_property (GObject      *object,
                      guint         prop_id,
                      const GValue *value,
                      GParamSpec   *pspec)
{
  GtkSeqBox *box = GTK_SEQ_BOX (object);
  GtkSeqBoxPrivate *private = GTK_SEQ_BOX_GET_PRIVATE (box);

  switch (prop_id)
    {
    case PROP_ORIENTATION:
      private->orientation = g_value_get_enum (value);
      gtk_widget_queue_resize (GTK_WIDGET (box));
      break;
    case PROP_SPACING:
      gtk_seq_box_set_spacing (box, g_value_get_int (value));
      break;
    case PROP_HOMOGENEOUS:
      gtk_seq_box_set_homogeneous (box, g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_seq_box_get_property (GObject    *object,
                      guint       prop_id,
                      GValue     *value,
                      GParamSpec *pspec)
{
  GtkSeqBox *box = GTK_SEQ_BOX (object);
  GtkSeqBoxPrivate *private = GTK_SEQ_BOX_GET_PRIVATE (box);

  switch (prop_id)
    {
    case PROP_ORIENTATION:
      g_value_set_enum (value, private->orientation);
      break;
    case PROP_SPACING:
      g_value_set_int (value, box->spacing);
      break;
    case PROP_HOMOGENEOUS:
      g_value_set_boolean (value, box->homogeneous);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_seq_box_size_request (GtkWidget      *widget,
                      GtkRequisition *requisition)
{
  GtkSeqBox *box = GTK_SEQ_BOX (widget);
  GtkSeqBoxPrivate *private = GTK_SEQ_BOX_GET_PRIVATE (box);
  GtkSeqBoxChild *child;
  GSequence *children;
  GSequenceIter * iter;
  gint nvis_children;
  gint width;
  gint height;

  requisition->width = 0;
  requisition->height = 0;
  nvis_children = 0;

  children = box->children;
  iter = g_sequence_get_begin_iter(children);
  while (!g_sequence_iter_is_end(iter))
    {
      child = g_sequence_get(iter);
      iter = g_sequence_iter_next(iter);

      if (GTK_WIDGET_VISIBLE (child->widget))
	{
	  GtkRequisition child_requisition;

	  gtk_widget_size_request (child->widget, &child_requisition);

	  if (box->homogeneous)
	    {
	      width = child_requisition.width + child->padding * 2;
	      height = child_requisition.height + child->padding * 2;

              if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
                requisition->width = MAX (requisition->width, width);
              else
                requisition->height = MAX (requisition->height, height);
	    }
	  else
	    {
              if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
                requisition->width += child_requisition.width + child->padding * 2;
              else
                requisition->height += child_requisition.height + child->padding * 2;
	    }

          if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
            requisition->height = MAX (requisition->height, child_requisition.height);
          else
            requisition->width = MAX (requisition->width, child_requisition.width);

	  nvis_children += 1;
	}
    }

  if (nvis_children > 0)
    {
      if (box->homogeneous)
        {
          if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
            requisition->width *= nvis_children;
          else
            requisition->height *= nvis_children;
        }

      if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
        requisition->width += (nvis_children - 1) * box->spacing;
      else
        requisition->height += (nvis_children - 1) * box->spacing;
    }

  requisition->width += GTK_CONTAINER (box)->border_width * 2;
  requisition->height += GTK_CONTAINER (box)->border_width * 2;
}

static void
gtk_seq_box_size_allocate (GtkWidget     *widget,
                       GtkAllocation *allocation)
{
  GtkSeqBox *box = GTK_SEQ_BOX (widget);
  GtkSeqBoxPrivate *private = GTK_SEQ_BOX_GET_PRIVATE (box);
  GtkSeqBoxChild *child;
  GSequenceIter * iter;
  GtkAllocation child_allocation;
  gint nvis_children = 0;
  gint nexpand_children = 0;
  gint child_width = 0;
  gint child_height = 0;
  gint width = 0;
  gint height = 0;
  gint extra = 0;
  gint x = 0;
  gint y = 0;
  GtkTextDirection direction;

  widget->allocation = *allocation;

  direction = gtk_widget_get_direction (widget);

  iter = g_sequence_get_begin_iter(box->children);
  for (iter = g_sequence_get_begin_iter(box->children); !g_sequence_iter_is_end(iter); iter = g_sequence_iter_next(iter))
    {
      child = g_sequence_get(iter);

      if (GTK_WIDGET_VISIBLE (child->widget))
	{
	  nvis_children += 1;
	  if (child->expand)
	    nexpand_children += 1;
	}
    }

  if (nvis_children > 0)
    {
      if (box->homogeneous)
	{
          if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
            {
              width = (allocation->width -
                       GTK_CONTAINER (box)->border_width * 2 -
                       (nvis_children - 1) * box->spacing);
              extra = width / nvis_children;
            }
          else
            {
              height = (allocation->height -
                        GTK_CONTAINER (box)->border_width * 2 -
                        (nvis_children - 1) * box->spacing);
              extra = height / nvis_children;
            }
	}
      else if (nexpand_children > 0)
	{
          if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
            {
              width = (gint) allocation->width - (gint) widget->requisition.width;
              extra = width / nexpand_children;
            }
          else
            {
              height = (gint) allocation->height - (gint) widget->requisition.height;
              extra = height / nexpand_children;
            }
	}

      if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
        {
          x = allocation->x + GTK_CONTAINER (box)->border_width;
          child_allocation.y = allocation->y + GTK_CONTAINER (box)->border_width;
          child_allocation.height = MAX (1, (gint) allocation->height - (gint) GTK_CONTAINER (box)->border_width * 2);
        }
      else
        {
          y = allocation->y + GTK_CONTAINER (box)->border_width;
          child_allocation.x = allocation->x + GTK_CONTAINER (box)->border_width;
          child_allocation.width = MAX (1, (gint) allocation->width - (gint) GTK_CONTAINER (box)->border_width * 2);
        }

      iter = g_sequence_get_begin_iter(box->children);
      while (!g_sequence_iter_is_end(iter))
	{
	  child = g_sequence_get(iter);
	  iter = g_sequence_iter_next(iter);

	  if ((child->pack == GTK_PACK_START) && GTK_WIDGET_VISIBLE (child->widget))
	    {
	      if (box->homogeneous)
		{
		  if (nvis_children == 1)
                    {
                      child_width = width;
                      child_height = height;
                    }
		  else
                    {
                      child_width = extra;
                      child_height = extra;
                    }

		  nvis_children -= 1;
		  width -= extra;
                  height -= extra;
		}
	      else
		{
		  GtkRequisition child_requisition;

		  gtk_widget_get_child_requisition (child->widget, &child_requisition);

		  child_width = child_requisition.width + child->padding * 2;
		  child_height = child_requisition.height + child->padding * 2;

		  if (child->expand)
		    {
		      if (nexpand_children == 1)
                        {
                          child_width += width;
                          child_height += height;
                        }
		      else
                        {
                          child_width += extra;
                          child_height += extra;
                        }

		      nexpand_children -= 1;
		      width -= extra;
                      height -= extra;
		    }
		}

	      if (child->fill)
		{
                  if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
                    {
                      child_allocation.width = MAX (1, (gint) child_width - (gint) child->padding * 2);
                      child_allocation.x = x + child->padding;
                    }
                  else
                    {
                      child_allocation.height = MAX (1, child_height - (gint)child->padding * 2);
                      child_allocation.y = y + child->padding;
                    }
		}
	      else
		{
		  GtkRequisition child_requisition;

		  gtk_widget_get_child_requisition (child->widget, &child_requisition);
                  if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
                    {
                      child_allocation.width = child_requisition.width;
                      child_allocation.x = x + (child_width - child_allocation.width) / 2;
                    }
                  else
                    {
                      child_allocation.height = child_requisition.height;
                      child_allocation.y = y + (child_height - child_allocation.height) / 2;
                    }
		}

	      if (direction == GTK_TEXT_DIR_RTL &&
                  private->orientation == GTK_ORIENTATION_HORIZONTAL)
                {
                  child_allocation.x = allocation->x + allocation->width - (child_allocation.x - allocation->x) - child_allocation.width;
                }

	      gtk_widget_size_allocate (child->widget, &child_allocation);

	      x += child_width + box->spacing;
	      y += child_height + box->spacing;
	    }
	}

      x = allocation->x + allocation->width - GTK_CONTAINER (box)->border_width;
      y = allocation->y + allocation->height - GTK_CONTAINER (box)->border_width;

      iter = g_sequence_get_begin_iter(box->children);
      while (!g_sequence_iter_is_end(iter))
	{
	  child = g_sequence_get(iter);
	  iter = g_sequence_iter_next(iter);

	  if ((child->pack == GTK_PACK_END) && GTK_WIDGET_VISIBLE (child->widget))
	    {
	      GtkRequisition child_requisition;

	      gtk_widget_get_child_requisition (child->widget, &child_requisition);

              if (box->homogeneous)
                {
                  if (nvis_children == 1)
                    {
                      child_width = width;
                      child_height = height;
                    }
                  else
                    {
                      child_width = extra;
                      child_height = extra;
                   }

                  nvis_children -= 1;
                  width -= extra;
                  height -= extra;
                }
              else
                {
		  child_width = child_requisition.width + child->padding * 2;
		  child_height = child_requisition.height + child->padding * 2;

                  if (child->expand)
                    {
                      if (nexpand_children == 1)
                        {
                          child_width += width;
                          child_height += height;
                         }
                      else
                        {
                          child_width += extra;
                          child_height += extra;
                        }

                      nexpand_children -= 1;
                      width -= extra;
                      height -= extra;
                    }
                }

              if (child->fill)
                {
                  if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
                    {
                      child_allocation.width = MAX (1, (gint)child_width - (gint)child->padding * 2);
                      child_allocation.x = x + child->padding - child_width;
                    }
                  else
                    {
                      child_allocation.height = MAX (1, child_height - (gint)child->padding * 2);
                      child_allocation.y = y + child->padding - child_height;
                     }
                }
              else
                {
                  if (private->orientation == GTK_ORIENTATION_HORIZONTAL)
                    {
                      child_allocation.width = child_requisition.width;
                      child_allocation.x = x + (child_width - child_allocation.width) / 2 - child_width;
                    }
                  else
                    {
                      child_allocation.height = child_requisition.height;
                      child_allocation.y = y + (child_height - child_allocation.height) / 2 - child_height;
                    }
                }

	      if (direction == GTK_TEXT_DIR_RTL &&
                  private->orientation == GTK_ORIENTATION_HORIZONTAL)
                {
                  child_allocation.x = allocation->x + allocation->width - (child_allocation.x - allocation->x) - child_allocation.width;
                }

              gtk_widget_size_allocate (child->widget, &child_allocation);

              x -= (child_width + box->spacing);
              y -= (child_height + box->spacing);
	    }
	}
    }
}

static GType
gtk_seq_box_child_type (GtkContainer   *container)
{
  return GTK_TYPE_WIDGET;
}

static void
gtk_seq_box_set_child_property (GtkContainer *container,
                            GtkWidget    *child,
                            guint         property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  gboolean expand = 0;
  gboolean fill = 0;
  guint padding = 0;
  GtkPackType pack_type = 0;

  if (property_id != CHILD_PROP_POSITION)
    gtk_seq_box_query_child_packing (GTK_SEQ_BOX (container),
				 child,
				 &expand,
				 &fill,
				 &padding,
				 &pack_type);
  switch (property_id)
    {
    case CHILD_PROP_EXPAND:
      gtk_seq_box_set_child_packing (GTK_SEQ_BOX (container),
				 child,
				 g_value_get_boolean (value),
				 fill,
				 padding,
				 pack_type);
      break;
    case CHILD_PROP_FILL:
      gtk_seq_box_set_child_packing (GTK_SEQ_BOX (container),
				 child,
				 expand,
				 g_value_get_boolean (value),
				 padding,
				 pack_type);
      break;
    case CHILD_PROP_PADDING:
      gtk_seq_box_set_child_packing (GTK_SEQ_BOX (container),
				 child,
				 expand,
				 fill,
				 g_value_get_uint (value),
				 pack_type);
      break;
    case CHILD_PROP_PACK_TYPE:
      gtk_seq_box_set_child_packing (GTK_SEQ_BOX (container),
				 child,
				 expand,
				 fill,
				 padding,
				 g_value_get_enum (value));
      break;
    case CHILD_PROP_POSITION:
      gtk_seq_box_reorder_child (GTK_SEQ_BOX (container),
			     child,
			     g_value_get_int (value));
      break;
    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      break;
    }
}

static void
gtk_seq_box_get_child_property (GtkContainer *container,
			    GtkWidget    *child,
			    guint         property_id,
			    GValue       *value,
			    GParamSpec   *pspec)
{
  gboolean expand = 0;
  gboolean fill = 0;
  guint padding = 0;
  GtkPackType pack_type = 0;
  GSequenceIter *iter;

  if (property_id != CHILD_PROP_POSITION)
    gtk_seq_box_query_child_packing (GTK_SEQ_BOX (container),
				 child,
				 &expand,
				 &fill,
				 &padding,
				 &pack_type);
  switch (property_id)
    {
      guint i;
    case CHILD_PROP_EXPAND:
      g_value_set_boolean (value, expand);
      break;
    case CHILD_PROP_FILL:
      g_value_set_boolean (value, fill);
      break;
    case CHILD_PROP_PADDING:
      g_value_set_uint (value, padding);
      break;
    case CHILD_PROP_PACK_TYPE:
      g_value_set_enum (value, pack_type);
      break;
    case CHILD_PROP_POSITION:
      i = 0;
      for (iter = g_sequence_get_begin_iter(GTK_SEQ_BOX(container)->children); 
      		!g_sequence_iter_is_end(iter); iter = g_sequence_iter_next(iter))
        {
	  GtkSeqBoxChild *child_entry;

          child_entry = g_sequence_get(iter);
	  if (child_entry->widget == child)
	    break;
	  i++;
	}
      g_value_set_int (value, !g_sequence_iter_is_end(iter) ? i : -1);
      break;
    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      break;
    }
}

static void
gtk_seq_box_pack (GtkSeqBox      *box,
              GtkWidget   *child,
              gboolean     expand,
              gboolean     fill,
              guint        padding,
              GtkPackType  pack_type)
{
  GtkSeqBoxChild *child_info;

  g_return_if_fail (GTK_IS_SEQ_BOX (box));
  g_return_if_fail (GTK_IS_WIDGET (child));
  g_return_if_fail (child->parent == NULL);

  child_info = g_new (GtkSeqBoxChild, 1);
  child_info->widget = child;
  child_info->padding = padding;
  child_info->expand = expand ? TRUE : FALSE;
  child_info->fill = fill ? TRUE : FALSE;
  child_info->pack = pack_type;
  child_info->is_secondary = FALSE;

  GSequenceIter * iter = g_sequence_get_end_iter(box->children);
  g_sequence_insert_before(iter, child_info);

  gtk_widget_freeze_child_notify (child);

  gtk_widget_set_parent (child, GTK_WIDGET (box));
  
  gtk_widget_child_notify (child, "expand");
  gtk_widget_child_notify (child, "fill");
  gtk_widget_child_notify (child, "padding");
  gtk_widget_child_notify (child, "pack-type");
  gtk_widget_child_notify (child, "position");
  gtk_widget_thaw_child_notify (child);
}

/**
 * gtk_seq_box_new:
 * @orientation: the box' orientation.
 * @homogeneous: %TRUE if all children are to be given equal space allocations.
 * @spacing: the number of pixels to place by default between children.
 *
 * Creates a new #GtkHBox.
 *
 * Return value: a new #GtkHBox.
 *
 * Since: 2.16
 **/
GtkWidget* _gtk_seq_box_new                 (GtkOrientation  orientation,
                                             gboolean        homogeneous,
                                             gint            spacing)
{
  return g_object_new (GTK_TYPE_SEQ_BOX,
                       "orientation", orientation,
                       "spacing",     spacing,
                       "homogeneous", homogeneous ? TRUE : FALSE,
                       NULL);
}

int gtk_seq_box_compare_child_info(gconstpointer pA, gconstpointer pB, gpointer pbox)
{
	GtkSeqBox * box = (GtkSeqBox*) pbox;
	GtkSeqBoxChild * ciA = (GtkSeqBoxChild*) pA;
	GtkSeqBoxChild * ciB = (GtkSeqBoxChild*) pB;
	return box -> cmp_func(ciA -> key, ciB -> key, box -> cmp_data);
}

int gtk_seq_box_compare_inverse_entries(gconstpointer pA, gconstpointer pB, gpointer data)
{
	InverseEntry * eA = (InverseEntry*) pA;
	InverseEntry * eB = (InverseEntry*) pB;
	return (((gpointer)eA -> child) - ((gpointer)eB -> child));
}

void gtk_seq_box_insert	(GtkSeqBox * box,
			 gpointer key,
			 GtkWidget * child,
			 gboolean expand,
			 gboolean fill,
			 guint padding)
{
  GtkSeqBoxChild *child_info;

  g_return_if_fail (GTK_IS_SEQ_BOX (box));
  g_return_if_fail (GTK_IS_WIDGET (child));
  g_return_if_fail (child->parent == NULL);

  child_info = g_new (GtkSeqBoxChild, 1);
  child_info->widget = child;
  child_info->key = key;
  child_info->padding = padding;
  child_info->expand = expand ? TRUE : FALSE;
  child_info->fill = fill ? TRUE : FALSE;
  child_info->pack = GTK_PACK_START;
  child_info->is_secondary = FALSE;

  g_sequence_insert_sorted(box -> children, child_info, gtk_seq_box_compare_child_info, box);

  InverseEntry * ie = malloc(sizeof(InverseEntry));
  ie -> key = key;
  ie -> child = child;
  g_sequence_insert_sorted(box -> children_inverse, ie, gtk_seq_box_compare_inverse_entries, NULL);

  gtk_widget_freeze_child_notify (child);

  gtk_widget_set_parent (child, GTK_WIDGET (box));
  
  gtk_widget_child_notify (child, "expand");
  gtk_widget_child_notify (child, "fill");
  gtk_widget_child_notify (child, "padding");
  gtk_widget_child_notify (child, "pack-type");
  gtk_widget_child_notify (child, "position");
  gtk_widget_thaw_child_notify (child);

}


void gtk_seq_box_reorder (GtkSeqBox * box,
			  gpointer key_old,
			  gpointer key_new)
{
  	GtkSeqBoxChild child_info;
	child_info.key = key_old;
	GSequenceIter * iter = g_sequence_find(box -> children, &child_info, gtk_seq_box_compare_child_info, box);
	if (iter != NULL){
		GtkSeqBoxChild * ci = (GtkSeqBoxChild*)g_sequence_get(iter);
		ci -> key = key_new;
		g_sequence_sort_changed(iter, gtk_seq_box_compare_child_info, box);

		InverseEntry ie;
		ie.child = ci -> widget;
		GSequenceIter * iter2 
			= g_sequence_find(box -> children_inverse, &ie, gtk_seq_box_compare_inverse_entries, NULL);
		if (iter2 != NULL){
			InverseEntry * e = (InverseEntry*)g_sequence_get(iter2);
			e -> key = key_new;
		}

		gtk_widget_child_notify (ci -> widget, "position");
		if (GTK_WIDGET_VISIBLE (ci -> widget) && GTK_WIDGET_VISIBLE (box))
			gtk_widget_queue_resize (ci -> widget);
	}
}


void gtk_seq_remove	(GtkSeqBox * box,
			 gpointer key)
{
  	GtkSeqBoxChild child_info;
	child_info.key = key;
	GSequenceIter * iter = g_sequence_find(box -> children, &child_info, gtk_seq_box_compare_child_info, box);
	if (iter != NULL){
		GtkSeqBoxChild * ci = (GtkSeqBoxChild*)g_sequence_get(iter);

		InverseEntry ie;
		ie.child = ci -> widget;
		GSequenceIter * iter_inverse = 
			g_sequence_find(box -> children_inverse, &ie, gtk_seq_box_compare_inverse_entries, NULL);
		if (iter_inverse != NULL){
			g_sequence_remove(iter_inverse);
		}

		g_sequence_remove(iter);
		gboolean was_visible = GTK_WIDGET_VISIBLE (ci -> widget);
		gtk_widget_unparent (ci -> widget);
		if (was_visible)
			gtk_widget_queue_resize (GTK_WIDGET(box));
	}
}

GtkWidget * gtk_seq_box_get	(GtkSeqBox * box,
			 	 gpointer key)
{
	GtkWidget * widget = NULL;
  	GtkSeqBoxChild child_info;
	child_info.key = key;
	GSequenceIter * iter = g_sequence_find(box -> children, &child_info, gtk_seq_box_compare_child_info, box);
	if (iter != NULL){
		GtkSeqBoxChild * ci = (GtkSeqBoxChild*)g_sequence_get(iter);
		widget = ci -> widget;
	}
	return widget;
}

gpointer gtk_seq_box_get_key	(GtkSeqBox * box,
				 GtkWidget * child)
{
	InverseEntry ie;
	ie.child = child;
	GSequenceIter * iter = 
		g_sequence_find(box -> children_inverse, &ie, gtk_seq_box_compare_inverse_entries, NULL);
	if (iter == NULL){
		return NULL;
	}
	InverseEntry * fie = (InverseEntry*) g_sequence_get(iter);
	gpointer key = fie -> key;
	return key;
}

void gtk_seq_box_set_compare_function	(GtkSeqBox * box,
					 GCompareDataFunc cmp_func)
{
	box -> cmp_func = cmp_func;
}

void gtk_seq_box_set_compare_data	(GtkSeqBox * box,
					 gpointer cmp_data)
{
	box -> cmp_data = cmp_data;
}

void gtk_seq_box_set_destroy_function	(GtkSeqBox * box,
					 GDestroyNotify key_destroy)
{
	box -> key_destroy = key_destroy;
}

/**
 * gtk_seq_box_pack_start:
 * @box: a #GtkSeqBox
 * @child: the #GtkWidget to be added to @box
 * @expand: %TRUE if the new child is to be given extra space allocated to
 * @box.  The extra space will be divided evenly between all children of
 * @box that use this option
 * @fill: %TRUE if space given to @child by the @expand option is
 *   actually allocated to @child, rather than just padding it.  This
 *   parameter has no effect if @expand is set to %FALSE.  A child is
 *   always allocated the full height of a #GtkHBox and the full width 
 *   of a #GtkVBox. This option affects the other dimension
 * @padding: extra space in pixels to put between this child and its
 *   neighbors, over and above the global amount specified by
 *   #GtkSeqBox:spacing property.  If @child is a widget at one of the 
 *   reference ends of @box, then @padding pixels are also put between 
 *   @child and the reference edge of @box
 *
 * Adds @child to @box, packed with reference to the start of @box.
 * The @child is packed after any other child packed with reference 
 * to the start of @box.
 */
void
gtk_seq_box_pack_start (GtkSeqBox    *box,
		    GtkWidget *child,
		    gboolean   expand,
		    gboolean   fill,
		    guint      padding)
{
  gtk_seq_box_pack (box, child, expand, fill, padding, GTK_PACK_START);
}

/**
 * gtk_seq_box_pack_end:
 * @box: a #GtkSeqBox
 * @child: the #GtkWidget to be added to @box
 * @expand: %TRUE if the new child is to be given extra space allocated 
 *   to @box. The extra space will be divided evenly between all children 
 *   of @box that use this option
 * @fill: %TRUE if space given to @child by the @expand option is
 *   actually allocated to @child, rather than just padding it.  This
 *   parameter has no effect if @expand is set to %FALSE.  A child is
 *   always allocated the full height of a #GtkHBox and the full width 
 *   of a #GtkVBox.  This option affects the other dimension
 * @padding: extra space in pixels to put between this child and its
 *   neighbors, over and above the global amount specified by
 *   #GtkSeqBox:spacing property.  If @child is a widget at one of the 
 *   reference ends of @box, then @padding pixels are also put between 
 *   @child and the reference edge of @box
 *
 * Adds @child to @box, packed with reference to the end of @box.  
 * The @child is packed after (away from end of) any other child 
 * packed with reference to the end of @box.
 */
void
gtk_seq_box_pack_end (GtkSeqBox    *box,
		  GtkWidget *child,
		  gboolean   expand,
		  gboolean   fill,
		  guint      padding)
{
  gtk_seq_box_pack (box, child, expand, fill, padding, GTK_PACK_END);
}

/**
 * gtk_seq_box_pack_start_defaults:
 * @box: a #GtkSeqBox
 * @widget: the #GtkWidget to be added to @box
 *
 * Adds @widget to @box, packed with reference to the start of @box.
 * The child is packed after any other child packed with reference 
 * to the start of @box. 
 * 
 * Parameters for how to pack the child @widget, #GtkSeqBox:expand, 
 * #GtkSeqBox:fill and #GtkSeqBox:padding, are given their default
 * values, %TRUE, %TRUE, and 0, respectively.
 *
 * Deprecated: 2.14: Use gtk_seq_box_pack_start()
 */
void
gtk_seq_box_pack_start_defaults (GtkSeqBox    *box,
			     GtkWidget *child)
{
  gtk_seq_box_pack_start (box, child, TRUE, TRUE, 0);
}

/**
 * gtk_seq_box_pack_end_defaults:
 * @box: a #GtkSeqBox
 * @widget: the #GtkWidget to be added to @box
 *
 * Adds @widget to @box, packed with reference to the end of @box.
 * The child is packed after any other child packed with reference 
 * to the start of @box. 
 * 
 * Parameters for how to pack the child @widget, #GtkSeqBox:expand, 
 * #GtkSeqBox:fill and #GtkSeqBox:padding, are given their default
 * values, %TRUE, %TRUE, and 0, respectively.
 *
 * Deprecated: 2.14: Use gtk_seq_box_pack_end()
 */
void
gtk_seq_box_pack_end_defaults (GtkSeqBox    *box,
			   GtkWidget *child)
{
  gtk_seq_box_pack_end (box, child, TRUE, TRUE, 0);
}

/**
 * gtk_seq_box_set_homogeneous:
 * @box: a #GtkSeqBox
 * @homogeneous: a boolean value, %TRUE to create equal allotments,
 *   %FALSE for variable allotments
 * 
 * Sets the #GtkSeqBox:homogeneous property of @box, controlling 
 * whether or not all children of @box are given equal space 
 * in the box.
 */
void
gtk_seq_box_set_homogeneous (GtkSeqBox  *box,
			 gboolean homogeneous)
{
  g_return_if_fail (GTK_IS_SEQ_BOX (box));

  if ((homogeneous ? TRUE : FALSE) != box->homogeneous)
    {
      box->homogeneous = homogeneous ? TRUE : FALSE;
      g_object_notify (G_OBJECT (box), "homogeneous");
      gtk_widget_queue_resize (GTK_WIDGET (box));
    }
}

/**
 * gtk_seq_box_get_homogeneous:
 * @box: a #GtkSeqBox
 *
 * Returns whether the box is homogeneous (all children are the
 * same size). See gtk_seq_box_set_homogeneous().
 *
 * Return value: %TRUE if the box is homogeneous.
 **/
gboolean
gtk_seq_box_get_homogeneous (GtkSeqBox *box)
{
  g_return_val_if_fail (GTK_IS_SEQ_BOX (box), FALSE);

  return box->homogeneous;
}

/**
 * gtk_seq_box_set_spacing:
 * @box: a #GtkSeqBox
 * @spacing: the number of pixels to put between children
 *
 * Sets the #GtkSeqBox:spacing property of @box, which is the 
 * number of pixels to place between children of @box.
 */
void
gtk_seq_box_set_spacing (GtkSeqBox *box,
		     gint    spacing)
{
  g_return_if_fail (GTK_IS_SEQ_BOX (box));

  if (spacing != box->spacing)
    {
      box->spacing = spacing;
      _gtk_seq_box_set_spacing_set (box, TRUE);

      g_object_notify (G_OBJECT (box), "spacing");

      gtk_widget_queue_resize (GTK_WIDGET (box));
    }
}

/**
 * gtk_seq_box_get_spacing:
 * @box: a #GtkSeqBox
 * 
 * Gets the value set by gtk_seq_box_set_spacing().
 * 
 * Return value: spacing between children
 **/
gint
gtk_seq_box_get_spacing (GtkSeqBox *box)
{
  g_return_val_if_fail (GTK_IS_SEQ_BOX (box), 0);

  return box->spacing;
}

void
_gtk_seq_box_set_spacing_set (GtkSeqBox  *box,
                          gboolean spacing_set)
{
  GtkSeqBoxPrivate *private;

  g_return_if_fail (GTK_IS_SEQ_BOX (box));

  private = GTK_SEQ_BOX_GET_PRIVATE (box);

  private->spacing_set = spacing_set ? TRUE : FALSE;
}

gboolean
_gtk_seq_box_get_spacing_set (GtkSeqBox *box)
{
  GtkSeqBoxPrivate *private;

  g_return_val_if_fail (GTK_IS_SEQ_BOX (box), FALSE);

  private = GTK_SEQ_BOX_GET_PRIVATE (box);

  return private->spacing_set;
}

/**
 * gtk_seq_box_reorder_child:
 * @box: a #GtkSeqBox
 * @child: the #GtkWidget to move
 * @position: the new position for @child in the list of children 
 *   of @box, starting from 0. If negative, indicates the end of 
 *   the list
 *
 * Moves @child to a new @position in the list of @box children.  
 * The list is the <structfield>children</structfield> field of
 * #GtkSeqBox-struct, and contains both widgets packed #GTK_PACK_START 
 * as well as widgets packed #GTK_PACK_END, in the order that these 
 * widgets were added to @box.
 * 
 * A widget's position in the @box children list determines where 
 * the widget is packed into @box.  A child widget at some position 
 * in the list will be packed just after all other widgets of the 
 * same packing type that appear earlier in the list.
 */ 
void
gtk_seq_box_reorder_child (GtkSeqBox    *box,
		       GtkWidget *child,
		       gint       position)
{ //TODO
//  GList *old_link;
//  GList *new_link;
//  GtkSeqBoxChild *child_info = NULL;
//  gint old_position;
//
//  g_return_if_fail (GTK_IS_SEQ_BOX (box));
//  g_return_if_fail (GTK_IS_WIDGET (child));
//
//  old_link = box->children;
//  old_position = 0;
//  while (old_link)
//    {
//      child_info = old_link->data;
//      if (child_info->widget == child)
//	break;
//
//      old_link = old_link->next;
//      old_position++;
//    }
//
//  g_return_if_fail (old_link != NULL);
//
//  if (position == old_position)
//    return;
//
//  box->children = g_list_delete_link (box->children, old_link);
//
//  if (position < 0)
//    new_link = NULL;
//  else
//    new_link = g_list_nth (box->children, position);
//
//  box->children = g_list_insert_before (box->children, new_link, child_info);
//
//  gtk_widget_child_notify (child, "position");
//  if (GTK_WIDGET_VISIBLE (child) && GTK_WIDGET_VISIBLE (box))
//    gtk_widget_queue_resize (child);
}

/**
 * gtk_seq_box_query_child_packing:
 * @box: a #GtkSeqBox
 * @child: the #GtkWidget of the child to query
 * @expand: pointer to return location for #GtkSeqBox:expand child property 
 * @fill: pointer to return location for #GtkSeqBox:fill child property 
 * @padding: pointer to return location for #GtkSeqBox:padding child property 
 * @pack_type: pointer to return location for #GtkSeqBox:pack-type child property 
 * 
 * Obtains information about how @child is packed into @box.
 */
void
gtk_seq_box_query_child_packing (GtkSeqBox      *box,
			     GtkWidget   *child,
			     gboolean    *expand,
			     gboolean    *fill,
			     guint       *padding,
			     GtkPackType *pack_type)
{ //TODO
//  GList *list;
//  GtkSeqBoxChild *child_info = NULL;
//
//  g_return_if_fail (GTK_IS_SEQ_BOX (box));
//  g_return_if_fail (GTK_IS_WIDGET (child));
//
//  list = box->children;
//  while (list)
//    {
//      child_info = list->data;
//      if (child_info->widget == child)
//	break;
//
//      list = list->next;
//    }
//
//  if (list)
//    {
//      if (expand)
//	*expand = child_info->expand;
//      if (fill)
//	*fill = child_info->fill;
//      if (padding)
//	*padding = child_info->padding;
//      if (pack_type)
//	*pack_type = child_info->pack;
//    }
}

/**
 * gtk_seq_box_set_child_packing:
 * @box: a #GtkSeqBox
 * @child: the #GtkWidget of the child to set
 * @expand: the new value of the #GtkSeqBox:expand child property 
 * @fill: the new value of the #GtkSeqBox:fill child property
 * @padding: the new value of the #GtkSeqBox:padding child property
 * @pack_type: the new value of the #GtkSeqBox:pack-type child property
 *
 * Sets the way @child is packed into @box.
 */
void
gtk_seq_box_set_child_packing (GtkSeqBox      *box,
			   GtkWidget   *child,
			   gboolean     expand,
			   gboolean     fill,
			   guint        padding,
			   GtkPackType  pack_type)
{ //TODO
//  GList *list;
//  GtkSeqBoxChild *child_info = NULL;
//
//  g_return_if_fail (GTK_IS_SEQ_BOX (box));
//  g_return_if_fail (GTK_IS_WIDGET (child));
//
//  list = box->children;
//  while (list)
//    {
//      child_info = list->data;
//      if (child_info->widget == child)
//	break;
//
//      list = list->next;
//    }
//
//  gtk_widget_freeze_child_notify (child);
//  if (list)
//    {
//      child_info->expand = expand != FALSE;
//      gtk_widget_child_notify (child, "expand");
//      child_info->fill = fill != FALSE;
//      gtk_widget_child_notify (child, "fill");
//      child_info->padding = padding;
//      gtk_widget_child_notify (child, "padding");
//      if (pack_type == GTK_PACK_END)
//	child_info->pack = GTK_PACK_END;
//      else
//	child_info->pack = GTK_PACK_START;
//      gtk_widget_child_notify (child, "pack-type");
//
//      if (GTK_WIDGET_VISIBLE (child) && GTK_WIDGET_VISIBLE (box))
//	gtk_widget_queue_resize (child);
//    }
//  gtk_widget_thaw_child_notify (child);
}

void
_gtk_seq_box_set_old_defaults (GtkSeqBox *box)
{
  GtkSeqBoxPrivate *private;

  g_return_if_fail (GTK_IS_SEQ_BOX (box));

  private = GTK_SEQ_BOX_GET_PRIVATE (box);

  private->default_expand = TRUE;
}

static void
gtk_seq_box_add (GtkContainer *container,
	     GtkWidget    *widget)
{
  GtkSeqBoxPrivate *private = GTK_SEQ_BOX_GET_PRIVATE (container);

  gtk_seq_box_pack_start (GTK_SEQ_BOX (container), widget,
                      private->default_expand,
                      private->default_expand,
                      0);
}

static void
gtk_seq_box_remove (GtkContainer *container,
		GtkWidget    *widget)
{ //TODO
//  GtkSeqBox *box = GTK_SEQ_BOX (container);
//  GtkSeqBoxChild *child;
//  GList *children;
//
//  children = box->children;
//  while (children)
//    {
//      child = children->data;
//
//      if (child->widget == widget)
//	{
//	  gboolean was_visible;
//
//	  was_visible = GTK_WIDGET_VISIBLE (widget);
//	  gtk_widget_unparent (widget);
//
//	  box->children = g_list_remove_link (box->children, children);
//	  g_list_free (children);
//	  g_free (child);
//
//	  /* queue resize regardless of GTK_WIDGET_VISIBLE (container),
//	   * since that's what is needed by toplevels.
//	   */
//	  if (was_visible)
//	    gtk_widget_queue_resize (GTK_WIDGET (container));
//
//	  break;
//	}
//
//      children = children->next;
//    }
}

static void
gtk_seq_box_forall (GtkContainer *container,
		gboolean      include_internals,
		GtkCallback   callback,
		gpointer      callback_data)
{
  GtkSeqBox *box = GTK_SEQ_BOX (container);
  GtkSeqBoxChild *child;
  GSequence *children;
  GSequenceIter * iter;

  children = box->children;
  iter = g_sequence_get_begin_iter(children);
  while (!g_sequence_iter_is_end(iter))
    {
      child = g_sequence_get(iter);
      iter = g_sequence_iter_next(iter);

      if (child->pack == GTK_PACK_START)
	(* callback) (child->widget, callback_data);
    }

}

#define __GTK_SEQ_BOX_C__
