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

#ifndef _VEX_SINGLE_CONTAINER_H_
#define _VEX_SINGLE_CONTAINER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "vex_single.h"
#include "config/vex_config.h"
#include "config/vex_layered_config.h"

#define VEX_TYPE_VEX_SINGLE_CONTAINER           (vex_single_container_get_type ())
#define VEX_VEX_SINGLE_CONTAINER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), VEX_TYPE_VEX_SINGLE_CONTAINER, VexSingleContainer))
#define VEX_VEX_SINGLE_CONTAINER_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), VEX_TYPE_VEX_SINGLE_CONTAINER, VexSingleContainerClass))
#define VEX_IS_VEX_SINGLE_CONTAINER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VEX_TYPE_VEX_SINGLE_CONTAINER))
#define VEX_IS_VEX_SINGLE_CONTAINER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), VEX_TYPE_VEX_SINGLE_CONTAINER))
#define VEX_VEX_SINGLE_CONTAINER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VEX_TYPE_VEX_SINGLE_CONTAINER, VexSingleContainerClass))

GType vex_single_container_get_type (void) G_GNUC_CONST; 

typedef struct _VexSingleContainer        VexSingleContainer;
typedef struct _VexSingleContainerClass   VexSingleContainerClass;

typedef enum{
	PROFILE_FOUND_AND_ACTIVE,
	PROFILE_NOT_FOUND_FELL_BACK_TO_DEFAULT,
} ProfileState;

struct _VexSingleContainer
{
	GtkVBox parent;

	VexSingle * vex_single;
	GtkWidget * menu_profiles;
	GSList * menu_profiles_group;

	VexLayeredConfig * vlc;
	GSequence * profile_names;
	char * profile_name;
	ProfileState profile_state;
};

struct _VexSingleContainerClass
{
	GtkVBoxClass parent_class;

	//void (* function_name) (VexSingleContainer *vex_single_container);
};

GtkWidget * vex_single_container_new(VexLayeredConfig * vlc, char * profile_name, char * pwd);

VexSingle * vex_single_container_get_vex_single(VexSingleContainer * vsc);
GtkWidget * vex_single_container_get_menu_profiles(VexSingleContainer * vsc);
void vex_single_container_set_config(VexSingleContainer * vcs, VexLayeredConfig * vlc);

#endif /* _VEX_SINGLE_CONTAINER_H_ */
