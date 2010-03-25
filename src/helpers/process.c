#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>

char * process_get_pwd(pid_t pid)
{
	char * file = g_strdup_printf("/proc/%d/cwd", pid);

	size_t s = 128;
	char * buf;
	while (TRUE){
		buf = malloc(s + 1);
		size_t r = readlink(file, buf, s);
		if (r < 0){
			free(buf); buf = NULL; break;
		}
		if (r == s){
			free(buf);
			s *= 2;
			if (s > 1024 * 4){
				free(buf); buf = NULL; break;
			}
			continue;
		}
		buf[r] = '\0';
		break;
	}
	return buf;
}

GArray * process_get_child_processes(pid_t pid)
{
	return NULL;
}
