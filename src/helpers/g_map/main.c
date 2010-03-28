#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <glib.h>
#include <glib-object.h>

#include "g_map.h"

gint compare_strings_data(gconstpointer a, gconstpointer b, gpointer data)
{
	char * sa = (char*)a;
	char * sb = (char*)b;
	int c = strcmp(sa, sb);
	return c;
}

int main(int argc, char * argv[])
{
	g_type_init();

	printf("GMap test\n");

	GMap * map = g_map_new(compare_strings_data, GINT_TO_POINTER(23));
	g_map_put(map, g_strdup("foo"), g_strdup("bar"));
	g_map_put(map, g_strdup("x"), g_strdup("y"));

	char * a = (char*) g_map_get(map, "foo");
	char * b = (char*) g_map_get(map, "x");
	char * c = (char*) g_map_get(map, "a");

	if (a == NULL) printf("NULL\n"); else printf("%s\n", a);
	if (b == NULL) printf("NULL\n"); else printf("%s\n", b);
	if (c == NULL) printf("NULL\n"); else printf("%s\n", c);

	printf("Iteration:\n");
	GMapIter * i;
	for (i = g_map_get_begin_iter(map); !g_map_iter_is_end(i); i = g_map_iter_next(i)){
		gpointer key;
		gpointer val = g_map_iter_get(i, &key);
		printf("%s:%s\n", key, val);
	}
	
	return 0;
}
