#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

#include "../helpers/util.h"

#define PATH_RES "/usr/share/vexterm/"
#define PATH_ALT ""

#define DEBUG_PATHS 0

extern char * executable;

char * vex_term_get_config_file_global()
{
	return g_strconcat(PATH_RES, "config.xml", NULL);
}

char * vex_term_get_config_file_local()
{
	char * homedir = getenv("HOME");
	char * real = realpath(homedir, NULL);
	char * home = g_strconcat(real, "/.config/vexterm/", "config.xml", NULL);
	return home;
}

char * vex_term_find_ressource(char * path)
{
	/* first try the given path for ressources */
	char * s_res = g_strconcat(PATH_RES, path, NULL);
	#if DEBUG_PATHS
	printf("trying: %s\n", s_res);
	#endif
	if (file_exists(s_res)){
		return s_res;
	}
	free(s_res);
	
	/* now try relative to our executable */
	char * exec_dir = dirname(g_strdup(executable));
	char * s_alt = g_strconcat(exec_dir, PATH_ALT, "/", path, NULL);
	#if DEBUG_PATHS
	printf("exec dir: %s\n", exec_dir);
	#endif
	free(exec_dir);
	#if DEBUG_PATHS
	printf("trying: %s\n", s_alt);
	#endif
	if (file_exists(s_alt)){
		return s_alt;
	}
	free(s_alt);

	/* unable to find */
	return NULL;
}
