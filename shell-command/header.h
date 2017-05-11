/*
* Author: Dana Taylor, Nick Boyle, Michael Dargie
* Class: Linux / Unix Programming
* Assignment: final
* Due Date: Friday 12/9/16 @ 11:59PM
*
* Description: Implement cp, cat, and a shell that uses custom commands.
*
* Certification of Authenticity:
* We certify that this assignment is entirely our own work.
*/

#ifndef HEADER_H_
#define HEADER_H_


#define ENVVAR_NAME_MAX_LENGTH 255



extern int g_exit_status;


int run_command(char* line);


void line_to_args(char* line, int* rtn_argc, char*** rtn_argv);

char* expand_envvar(char* envvar);

int expand_dollar(char* in, char* out);

char* expand_line(char* line);

#endif

