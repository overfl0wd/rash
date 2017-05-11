

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <libgen.h>

#include "header.h"
#include "builtin.h"
#include "colors.h"



/* STRINGIFY1 turns a string of characters into a c string literal
 * STRINGIFY2 is needed as an additional layer of indirection in
 * order for STRINGIFY1 to work properly. If STRINGIFY1 was called
 * directly the macro would not be expanded properly. For example
 * STRINGIFY1(BIN_PATH) would expand to "BIN_PATH" where as
 * STRINGIFY2(BIN_PATH) would expand to the contents of BIN_PATH
 * as a c string literal, in our case it was
 * "/home/share/finalproject/bin".
 *****************************************************************/

#define STRINGIFY1(p1) #p1
#define STRINGIFY2(p1) STRINGIFY1(p1)


int main(int argc, char** argv)
{
    char* line;
    char* str;
    char temp[256];
    char prompt[256];

    g_exit_status = 0;

/* BIN_PATH is set in the makefile and passed to the program with cc -DBIN_PATH=... */
#ifdef BIN_PATH
    setenv("PATH", STRINGIFY2(BIN_PATH), 1);
#endif

#ifndef NDEBUG
    fprintf(stderr, "PATH=%s\n", getenv("PATH"));
#endif

    while (1)
    {
        str = getenv("PROMPT");

        if (str)
        {
            strcpy(prompt, str);
        }
        else
        {
            strcpy(prompt, LIGHT_CYAN);
            strcat(prompt, getenv("USER"));
            strcat(prompt, DEFAULT);
            strcat(prompt, "@");
            gethostname(temp, 255);
            strcat(prompt, LIGHT_GREEN);
            strcat(prompt, temp);
            strcat(prompt, DEFAULT);
            strcat(prompt, ":");
            strcat(prompt, LIGHT_BLUE);
            if (strcmp(getenv("PWD"), getenv("HOME")) == 0)
            {
                strcat(prompt, "~");
            }
            else
            {
                strcat(prompt, basename(getenv("PWD")));
            }
            strcat(prompt, DEFAULT);
            if (g_exit_status)
            {
                strcat(prompt, RED);
                strcat(prompt, " $ ");
                strcat(prompt, DEFAULT);
            }
            else
            {
                strcat(prompt, " $ ");
            }
        }

        line = readline(prompt);

        g_exit_status = run_command(line);

        free(line);
    }

    return g_exit_status;
}

