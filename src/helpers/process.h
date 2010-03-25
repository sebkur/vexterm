#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>

char * process_get_pwd(pid_t pid);

GArray * process_get_child_processes(pid_t pid);
