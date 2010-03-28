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

 /* NOTES:
  #1: tab-charachters; erase in line; they ignore colour-inversion. (seen in xterm and gnome-terminal)
 */

#define DEBUG_TOFILE 0
#define DEBUG_CHILD_PID 0
#define DEBUG_UTF8 0
#define DEBUG_UNKNOWN_ESCAPES 0 

#define BUFSIZE 10000
#define PREBUFFER 10

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

#include "../helpers/tool.h"
#include "../helpers/util.h"
#include "../helpers/process.h"
#include "iso2022.h"
#include "terminal.h"

G_DEFINE_TYPE (Terminal, terminal, G_TYPE_OBJECT);

enum
{
        PTY_EXITED,
        LAST_SIGNAL
};

static guint terminal_signals[LAST_SIGNAL] = { 0 };

void pty_thread_func(Terminal * terminal);

Terminal * terminal_new(char * pwd)
{
	Terminal * terminal = g_object_new(LIBVEX_TYPE_TERMINAL, NULL);

	terminal -> initial_pwd = pwd == NULL ? NULL : g_strdup(pwd);

	return terminal;
}

static void terminal_class_init(TerminalClass * class)
{
        terminal_signals[PTY_EXITED] = g_signal_new(
                "pty-exited",
                G_OBJECT_CLASS_TYPE (class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (TerminalClass, pty_exited),
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void terminal_init(Terminal * terminal)
{
	int n_rows = 20;
	int n_cols = 80;

	terminal -> n_rows = n_rows;
	terminal -> n_cols = n_cols;

	terminal -> state = 0;

	#if DEBUG_TOFILE
	terminal -> debug = open("debug.dbg", O_CREAT | O_TRUNC | O_WRONLY, 0755);
	#endif

	terminal -> dec_ckm = FALSE;

	terminal -> unhandled_data = NULL;
	terminal -> unhandled_data_len = 0;

	terminal -> current_csi.nums = g_array_new(FALSE, FALSE, sizeof(gpointer));
}

void terminal_start(Terminal * terminal)
{
	pthread_t thread;
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	size_t size = 1024 * 1024; // TODO: make configurable
	pthread_attr_setstacksize(&tattr, size);
	size_t gsize;
	pthread_attr_getstacksize(&tattr, &gsize);
	/*int t = */pthread_create(&thread, &tattr, (void*) pty_thread_func, terminal);
}

void terminal_stop(Terminal * terminal)
{
	pid_t pty = terminal -> pty_pid;
	pid_t pty_grp = getpgid(pty);
	if (pty_grp != -1){
		kill(-pty_grp, SIGHUP);
	}
}

void terminal_set_handler(Terminal * terminal, TerminalHandler * handler)
{
	terminal -> handler = handler;
}

int terminal_get_master(Terminal * terminal)
{
	return terminal -> master;
}

char * terminal_get_pwd(Terminal * terminal)
{
	pid_t pid = terminal -> pty_pid;
	char * pwd = process_get_pwd(pid);
	return pwd;
}

/****************************************************************
Parsing / Actions
****************************************************************/

//typedef struct{
//	char prefix;
//	GArray * nums;
//	gboolean first_digit;
//	char suffix1;
//	char suffix2;
//} Csi;

enum{
	STATE_Normal,
	STATE_Esc,		// \x1b
	STATE_Title,		// ]
	STATE_LeftBracket,	// (
	STATE_RightBracket,	// )
	STATE_Csi,		// [
	STATE_Csi_Prefix,	// ?,>
	STATE_Csi_Num,		// 0-9
	STATE_Csi_Suffix	// a char
}states;

void clear_current_csi(Terminal * terminal)
{
	terminal -> current_csi.prefix = '\0';
	terminal -> current_csi.suffix1 = '\0';
	terminal -> current_csi.suffix2 = '\0';
	terminal -> current_csi.first_digit = TRUE;
	g_array_free(terminal -> current_csi.nums, TRUE);
	terminal -> current_csi.nums = g_array_new(FALSE, FALSE, sizeof(gpointer));
}

void handle_number(Terminal * terminal, char c)
{
	if (terminal -> current_csi.first_digit){
		terminal -> current_csi.first_digit = FALSE;
		int n = c - 48;
		gpointer num = GINT_TO_POINTER(n);
		g_array_append_val(terminal -> current_csi.nums, num);
	}else{
		int n = c - 48;
		gpointer * p = &g_array_index(terminal -> current_csi.nums, gpointer, terminal -> current_csi.nums -> len - 1);
		int num = GPOINTER_TO_INT(*p);
		num = num * 10 + n;
		*p = GINT_TO_POINTER(num);
	}
}

void handle_semicolon(Terminal * terminal)
{
	if (terminal -> current_csi.first_digit){
		int n = 0; //TODO: right default values
		gpointer num = GINT_TO_POINTER(n);
		g_array_append_val(terminal -> current_csi.nums, num);
	}else{
		terminal -> current_csi.first_digit = TRUE;
	}
}

void handle_suffix(Terminal * terminal, char c)
{
	terminal -> current_csi.suffix1 = c;
	switch(c){
		case '\"': case '\'':{
			terminal -> state = STATE_Csi_Suffix;
			break;
		}
		default:{
			//TODO: emit an action since a sequence was processed
			terminal -> state = STATE_Normal;
			TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal -> handler);
			class -> handle_csi(terminal -> handler, &terminal -> current_csi);
			break;
		}
	}
}

void handle_second_suffix(Terminal * terminal, char c)
{
	//TODO: emit an action since a sequence was processed
	terminal -> current_csi.suffix2 = c;
	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal -> handler);
	class -> handle_csi(terminal -> handler, &terminal -> current_csi);
}

void parse_ascii(Terminal * terminal, char c)
{
	switch(terminal -> state){
		case STATE_Csi_Suffix:{
			handle_second_suffix(terminal, c);
			break;
		}
		case STATE_Csi_Num:
		case STATE_Csi_Prefix:{
			switch(c){
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':{
					handle_number(terminal, c);
					break;
				}
				case ';':{
					handle_semicolon(terminal);
					break;
				}
				default:{
					handle_suffix(terminal, c);
					break;
				}
			}
			break;
		}
		case STATE_LeftBracket: //TODO: G0 (vt102 uses registers for charsets)
		case STATE_RightBracket:{ //TODO: G1
			//handle Language setting
			terminal -> state = STATE_Normal;
			TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal -> handler);
			class -> set_charset(terminal -> handler, c);
			break;
		}
		case STATE_Csi:{
			switch(c){
				case '!':
				case '?':
				case '>':{
					terminal -> state = STATE_Csi_Prefix;
					terminal -> current_csi.prefix = c;
					break;
				}
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':{
					terminal -> state = STATE_Csi_Num;
					handle_number(terminal, c);
					break;
				}
				case ';':{
					terminal -> state = STATE_Csi_Num;
					handle_semicolon(terminal);
					break;
				}
				default:{
					handle_suffix(terminal, c);
					break;
				}
			}
			break;
		}
		case STATE_Title:{
			switch(c){
				case '\07':{
					terminal -> state = STATE_Normal;
					break;
				}
				default:{
					//printf("|oo|");
					break;
				}
			}
			break;
		}
		case STATE_Esc:{
			if (c <= 15) break;
			switch(c){
				case ']':{
					terminal -> state = STATE_Title;
					break;
				}
				case '[':{
					terminal -> state = STATE_Csi;
					clear_current_csi(terminal);
					break;
				}
				case '(':{
					terminal -> state = STATE_LeftBracket;
					break;
				}
				case ')':{
					terminal -> state = STATE_RightBracket;
					break;
				}
				case 'D':  // Index (IND)
				case 'M':  // Reverse Index (RI)
				case 'E':  // Next Line (NEL)
				case '7':  // Save Cursor (DECSC)
				case '8':  // Restore Cursor (DECRC)
				case '=':  // Application Keypad Mode (DECKPAM)
				case '>':  // Numeric Keypad Mode (DECKPNM)
				case 'N':  // Single Shift 2 (SS2)
				case 'O':  // Single Shift 3 (SS3)
				case 'c':  // Reset (RIS)
				case 'H':{ // Home Position ()
					terminal -> state = STATE_Normal;
					TerminalHandlerInterface * class = 
						LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal -> handler);
					class -> handle_escaped
						(terminal -> handler, c);
					break;
				}
				default:{
					#if DEBUG_UNKNOWN_ESCAPES
					printf("||ESC:%c(%d)||", c, c); fflush(NULL);
					#endif
					// goto to STATE_Normal so that we don't become scrambled
					terminal -> state = STATE_Normal;
					break;
				}
			}
			break;
		}
		case STATE_Normal:{
			switch(c){
				case '\x1b':{	// ^[ : ESC
					terminal -> state = STATE_Esc;
					//printf(",");
					break;
				}
				default:{
					TerminalHandlerInterface * class = 
						LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal -> handler);
					class -> handle_ascii
						(terminal -> handler, c);
					break;
				}
			}
		}
	}
}

int parse_utf_8(Terminal * terminal, char * buffer, int max)
{
	if (terminal -> state == STATE_Normal){
		gunichar ch = g_utf8_get_char_validated(buffer, max);
		if (ch == -1 || ch == -2){
			#if DEBUG_UTF8
			printf("utf8 mistake: %d\n", ch); fflush(NULL);
			#endif
//			ch = 15712189;
//			char * str = malloc(sizeof(char) * 7);
//			int l = g_unichar_to_utf8(ch, str);
//			str[l] = '\0';
			if (ch == -1){ // nothing we could do
				TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal -> handler);
				class -> handle_utf8(terminal -> handler, string_to_unichar("?"));
				return -1;
			}else if (ch == -2){
				return 0;
			}
		}

		//char * str = malloc(sizeof(char) * 7);
		//int l = g_unichar_to_utf8(ch, str);
		//str[l] = '\0';
		TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal -> handler);
		class -> handle_utf8(terminal -> handler, ch);
		//class -> handle_utf8(terminal -> handler, string_to_unichar("?"));
		//free(str);
		//return l;
		return 0;
	} //TODO: end escape sequence ?
	return 0;
}

/*
 * utf8-helper
 * check the next 'check' bytes for utf8-fitness
 */
int check_sanity(char * buffer, int max, int check)
{
	if (max < check) return 0;
	int i;
	for (i = 0; i < check; i++){
		char x = buffer[i+1];
		if ((x & 0xC0) != 0x80){
			return check;
		}
	}
	return -1;
}

/*
 * validate utf8
 * redirect to 'parse_ascii' or 'parse_utf8'
 */
int parse_char(Terminal * terminal, char c, char * buffer, int max)
{
	//printf("state: %d\n", terminal -> state);
	char cx = buffer[0];
	gboolean ascii 	= (cx & 0x80) == 0x00;
	if (ascii){
		//printf("1 "); fflush(NULL); 
		//printf("ascii\n");
		parse_ascii(terminal, c);
		return 1;
	}

	int followers = 0;
	gboolean bytes2	= (cx & 0xE0) == 0xC0;
	gboolean bytes3	= (cx & 0xF0) == 0xE0;
	gboolean bytes4	= (cx & 0xF8) == 0xF0;

	if (bytes2) { 
//		printf("utf 2\n"); fflush(NULL); 
		followers = 1;
	}else if (bytes3) { 
//		printf("utf 3\n"); fflush(NULL); 
		followers = 2; 
	}else if (bytes4) { 
//		printf("utf 4\n"); fflush(NULL); 
		followers = 3; 
	}else{
//		printf("utf insane\n"); 
		return 1;
	}

	//terminal -> state = STATE_Normal; //TODO: this was defenitely not OK (mocp unicode bug)
	int checked = check_sanity(buffer, max - 1, followers);
	if (checked == -1){
		/*int a = */parse_utf_8(terminal, buffer, max);
		return followers + 1;
	}
	if (checked == 0) return 0;
	return checked;
}

/*
 * align unicode chars
 * redirect to 'parse_char'
 */
void parse(Terminal * terminal, char * buf, int c)
{
	//printf("PARSING %d BYTES\n", c);

	/* if we got some data left, prepend it. */
	int y = 0;
	if (terminal -> unhandled_data != NULL){
		y = terminal -> unhandled_data_len;
		memcpy(&buf[PREBUFFER - y], terminal -> unhandled_data, y);
		c = c + y;
		/* no unhandled data left anymore */
		free(terminal -> unhandled_data);
		terminal -> unhandled_data = NULL;
		terminal -> unhandled_data_len = 0;
	}

	/* now parse it, unichar by unichar */
	int p = 0;
	int x;
	char * vbuf = &buf[PREBUFFER - y];
	while (p < c){
		x = vbuf[p];
//		printf("%d ", x);
//		if (x >= 0x30 && x <= 0x7a) printf("<%c> ", x);
//		fflush(NULL);
		int used = parse_char(terminal, x, &vbuf[p], c - p);
		if (used != 0){ // a char has been read successully
			p += used;
		}else{ // end of buffer reached, no valid char
			int leftover = c - p;
			#if DEBUG_UTF8
			printf("leftover: %d\n", leftover); fflush(NULL);
			#endif
			terminal -> unhandled_data = malloc(sizeof(char) * leftover);
			memcpy(terminal -> unhandled_data, &vbuf[p], leftover);
			terminal -> unhandled_data_len = leftover;
			break;
		}
	}
}

/****************************************************************
Pseudo terminal communication
****************************************************************/

gboolean terminal_idle_emit_pty_exited(Terminal * terminal)
{
	g_signal_emit (terminal, terminal_signals[PTY_EXITED], 0);
	return FALSE;
}

extern char ** environ;

/*
 * start pty
 * call 'parse' for each char read from pty
 */
void pty_thread_func(Terminal * terminal)
{
	char * command = "/bin/bash";

	int mfd = posix_openpt(O_RDWR | O_NOCTTY); //master
	terminal -> master = mfd;
	//printf("master fd: %d\n", mfd);
	grantpt(mfd);
	unlockpt(mfd);
	int flags = fcntl(mfd, F_GETFL);
	flags &= ~(O_NONBLOCK);
	fcntl(mfd, F_SETFL, flags);
	char * pn = ptsname(mfd);
	//printf("ptsname: %s\n", pn);

	TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal -> handler);
	class -> set_fd(terminal -> handler, terminal -> master);

	pid_t pid = vfork();
	if (pid == 0){
		close(mfd);
		setsid();
		setpgid(0, 0);
		int sfd = open(pn, O_RDWR);
		ioctl (sfd, TIOCSCTTY, 0);

		struct termios st;
		tcgetattr (sfd, &st);
		st.c_iflag &= ~(ISTRIP | IGNCR | INLCR | IXOFF);
		st.c_iflag |= (ICRNL | IGNPAR | BRKINT | IXON);
		st.c_cflag &= ~CSIZE;
		st.c_cflag |= CREAD | CS8;
		tcsetattr (sfd, TCSANOW, &st);

		/* environment */
		char ** iter; int ec = 0;
		for (iter = environ; *iter != NULL; iter++){
			//char * var = *iter;
			//printf("[%s]\n", var);
			ec++;
		}
		char ** env = malloc(sizeof(char*) * (ec + 2));
		int e1 = 0, e2 = 0;
		for (iter = environ; *iter != NULL; iter++){
			char * var = *iter;
			if (strcmp(var, "TERM") == 0){
			}else{
				env[e2] = g_strdup(environ[e1]);
				e2++;
			}
			e1++;
		}
		env[e2++] = g_strdup("TERM=xterm");
		env[e2] = NULL;

		/* working directory */
		if(terminal -> initial_pwd != NULL){
			chdir(terminal -> initial_pwd);
		}

		//printf("slave fd: %d\n", sfd);
		dup2(sfd, 0);
		dup2(sfd, 1);
		dup2(sfd, 2);
		if (sfd > 2) close(sfd);

		int argc = 1;
		char ** argv = malloc(sizeof(char*) * (argc+2));
		argv[0] = command;
		argv[argc] = NULL;
		//execvp(command, argv);
		execve(command, argv, env);
		exit(0);
	}
	#if DEBUG_CHILD_PID
	printf("proc id: %d\n", pid);
	#endif
	terminal -> pty_pid = pid;

	struct termios st;
	tcgetattr (mfd, &st);
	//st.c_lflag &= ~(ECHO);
	tcsetattr (mfd, TCSANOW, &st);

	struct winsize size;
	memset(&size, 0, sizeof(size));
//	size.ws_row = terminal -> n_rows;
//	size.ws_col = terminal -> n_cols;
//	ioctl(mfd, TIOCSWINSZ, &size);

	char buf[BUFSIZE + PREBUFFER];

	ssize_t c;
	while (1){
		TerminalHandlerInterface * class = LIBVEX_TERMINAL_HANDLER_GET_INTERFACE(terminal -> handler);

		c = read(mfd, &buf[PREBUFFER], BUFSIZE - 1);
		if (c <= 0) break;
		fflush(NULL);
		//struct timeval t1, t2;

		#if DEBUG_TOFILE
		write(terminal -> debug, buf, c);
		#endif
		class -> chunk_begin(terminal -> handler);

		//gettimeofday(&t1, NULL);
		parse(terminal, buf, c);
		//gettimeofday(&t2, NULL);
		//printf("PARSE TIME: %d\n", time_diff(&t1, &t2));
		#if DEBUG_UTF8 && DEBUG_TOFILE
		//char * msg = ">>END_OF_MSG<<";
		write(terminal -> debug, msg, strlen(msg));
		#endif
		class -> chunk_done(terminal -> handler);
	}
	g_idle_add((GSourceFunc) terminal_idle_emit_pty_exited, terminal);
}
