/*
 * Copyright (C) 2002,2003 Red Hat, Inc.
 *
 * This is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>
#include <gdk/gdkkeysyms.h>

struct iso2022_map {
	gulong from;
	gunichar to;
};

/* DEC Special Character and Line Drawing Set.  VT100 and higher (per XTerm
 * docs). */
static const struct iso2022_map iso2022_map_0[] = {
	{ 96, 0x25c6},	/* diamond */
	{'a', 0x2592},	/* checkerboard */
	{'b', 0x2409},	/* HT symbol */
	{'c', 0x240c},	/* FF symbol */
	{'d', 0x240d},	/* CR symbol */
	{'e', 0x240a},	/* LF symbol */
	{'f', 0x00b0},	/* degree */
	{'g', 0x00b1},	/* plus/minus */
	{'h', 0x2424},  /* NL symbol */
	{'i', 0x240b},  /* VT symbol */
	{'j', 0x2518},	/* downright corner */
	{'k', 0x2510},	/* upright corner */
	{'l', 0x250c},	/* upleft corner */
	{'m', 0x2514},	/* downleft corner */
	{'n', 0x253c},	/* cross */
	{'o', 0x23ba},  /* scan line 1/9 */
	{'p', 0x23bb},  /* scan line 3/9 */
	{'q', 0x2500},	/* horizontal line (also scan line 5/9) */
	{'r', 0x23bc},  /* scan line 7/9 */
	{'s', 0x23bd},  /* scan line 9/9 */
	{'t', 0x251c},	/* left t */
	{'u', 0x2524},	/* right t */
	{'v', 0x2534},	/* bottom t */
	{'w', 0x252c},	/* top t */
	{'x', 0x2502},	/* vertical line */
	{'y', 0x2264},  /* <= */
	{'z', 0x2265},  /* >= */
	{'{', 0x03c0},  /* pi */
	{'|', 0x2260},  /* not equal */
	{'}', 0x00a3},  /* pound currency sign */
	{'~', 0x00b7},	/* bullet */
};

static GHashTable * char_map_init(const struct iso2022_map *map, gssize length)
{
	int i;
	GHashTable * ret = g_hash_table_new(NULL, NULL);
	for (i = 0; i < length; i++) {
		g_hash_table_insert(ret,
			      GINT_TO_POINTER(map[i].from),
			      GINT_TO_POINTER(map[i].to));
	}
	return ret;
}

GHashTable * char_map_special()
{
	GHashTable * map = char_map_init(iso2022_map_0, G_N_ELEMENTS(iso2022_map_0));
	return map;
}
