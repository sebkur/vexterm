#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>

#include "csi.h"

/* Csi helper function */
int get_value_or_default_from_csi(Csi * csi, int def)
{
	int n = def;
	if (csi -> nums -> len >= 1){
		gpointer num = g_array_index(csi -> nums, gpointer, 0);
		n = GPOINTER_TO_INT(num);
	}
	return n;
}

