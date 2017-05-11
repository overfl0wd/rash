

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "header.h"
#include "builtin.h"


int g_exit_status = 0;


int run_command(char* line)
{
    pid_t pid;
    int child_status;
    int child_argc;
    char** child_argv;
    int i;
    int exit_status;

    line = expand_line(line);

    line_to_args(line, &child_argc, &child_argv);

#ifndef NDEBUG
    fprintf(stderr, "argc: %i\n", child_argc);
    fprintf(stderr, "argv:");
    for (i = 0; i < child_argc; i++)
    {
        fprintf(stderr, " %i:%s", i, child_argv[i]);
    }
    fprintf(stderr, "\n");
#endif

    if (strcmp(child_argv[0], "exit") == 0)
    {
        exit_status = builtin_exit(child_argc, child_argv);
        exit(exit_status);
    }
    else if (strcmp(child_argv[0], "cd") == 0)
    {
        exit_status = builtin_cd(child_argc, child_argv);
        return exit_status;
    }
    else if (strcmp(child_argv[0], "export") == 0)
    {
        exit_status = builtin_export(child_argc, child_argv);
        return exit_status;
    }
    else if (strcmp(child_argv[0], "unset") == 0)
    {
        exit_status = builtin_unset(child_argc, child_argv);
        return exit_status;
    }
    else if (strcmp(child_argv[0], "true") == 0)
    {
        exit_status = builtin_true(child_argc, child_argv);
    }
    else if (strcmp(child_argv[0], "false") == 0)
    {
        exit_status = builtin_false(child_argc, child_argv);
    }


    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        execvp(child_argv[0], child_argv);
        exit(1);
    }
    else
    {
        free(child_argv);
        free(line);
        wait(&child_status);
        exit_status = WEXITSTATUS(child_status);
    }

    return exit_status;
}

int make_subshell(char* line, char* output)
{
    int pid;
    int child_status;
    int pipefd[2];
    char buffer[256];
    int chars_read;

    pipe(pipefd);

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        close(pipefd[0]);    // close reading end in the child
        dup2(pipefd[1], 1);  // send stdout to the pipe
        close(pipefd[1]);    // this descriptor is no longer needed

        run_command(line);

        exit(1);
    }
    else
    {
        close(pipefd[1]);  // close the write end of the pipe in the parent

        wait(&child_status);

        do
        {
            chars_read = read(pipefd[0], buffer, 255);
            buffer[chars_read] = '\0';
            strcpy(output, buffer);
            output += chars_read;
        } while (chars_read);

        if (output[-1] == '\n')
        {
            output[-1] = '\0';
        }

        close(pipefd[0]);
    }

    return WEXITSTATUS(child_status);
}


/* this funciton will handle double quotes */
void line_to_args(char* line, int* rtn_argc, char*** rtn_argv)
{
    int line_len = strlen(line);
    int argc = 0;
    char** argv = 0;
    int i = 0;
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    int added_to_argv = 0;


    for (i = 0; i < line_len; i++)
    {
        if (line[i] == '\'' && !in_double_quotes)
        {
            in_single_quotes = !in_single_quotes;
            line[i] = '\0';
            continue;
        }
        else if (line[i] == '"' && !in_single_quotes)
        {
            in_double_quotes = !in_double_quotes;
            line[i] = '\0';
            continue;
        }

        if (!in_single_quotes && !in_double_quotes && line[i] == ' ')
        {
            line[i] = '\0';
        }
    }


    for (i = 0; i < line_len; i++)
    {
        if (line[i])
        {
            if (!added_to_argv)
            {
                argv = realloc(argv, (argc + 1) * sizeof(char*));
                argv[argc++] = line + i;
                added_to_argv = 1;
            }
        }
        else
        {
            added_to_argv = 0;
        }
    }


    argv = realloc(argv, (argc + 1) * sizeof(char*));
    argv[argc] = 0;

    *rtn_argc = argc;
    *rtn_argv = argv;
}

char* expand_envvar(char* envvar)
{
    static char s_str[64];
    char* str;

    if (strcmp(envvar, "?") == 0)
    {
        sprintf(s_str, "%i", g_exit_status);
        str = s_str;
    }
    else
    {
        str = getenv(envvar);
        if (!str)
        {
            s_str[0] = '\0';
            str = s_str;
        }
    }

    return str;
}


/* takes a string that starts with a dollar sign as arg1, puts the expanded var in arg2, returns number of characters in the expansion */
int expand_dollar(char* in, char* out)
{
    char envvar[ENVVAR_NAME_MAX_LENGTH + 1];
    char* begin;
    char* end;
    int len;
    int depth;
    char* temp;

    begin = in + 1;

    if (*begin == '{')
    {
        begin++;
        end = strchr(begin, '}');
        len = end - begin;
        strncpy(envvar, begin, len);
        envvar[len] = '\0';
        strcpy(out, expand_envvar(envvar));
        end++;
    }
    else if (*begin == '(')
    {
        end = begin;
        begin++;
        depth = 1;
        while (depth > 0)
        {
            end++;
            if (*end == '(')
            {
                depth++;
            }
            else if (*end == ')')
            {
                depth--;
            }
        }
        temp = malloc((end - begin) + 1);
        strncpy(temp, begin, end - begin);
        temp[end - begin] = '\0';
        make_subshell(temp, out);
        free(temp);
        end++;
    }
    else if (isalpha(*begin) || *begin == '_')
    {
        end = begin;
        while (isalnum(*end) || *end == '_')
        {
            end++;
        }
        len = end - begin;
        strncpy(envvar, begin, len);
        envvar[len] = '\0';
        strcpy(out, expand_envvar(envvar));
    }
    else if (ispunct(*begin) && *begin != '_')
    {
        end = begin + 1;
        len = end - begin;
        strncpy(envvar, begin, len);
        envvar[len] = '\0';
        strcpy(out, expand_envvar(envvar));
    }
    else
    {
        fprintf(stderr, "invalid expansion\n");
        end = in + 1;
    }

    return end - in;
}


char* expand_line(char* line)
{
    char new_line[8192];
    int new_line_len = 0;
    int i;
    int in_double_quotes = 0;
    int in_single_quotes = 0;

    for (i = 0; line[i]; i++)
    {
        if (line[i] == '"' && !in_single_quotes)
        {
            in_double_quotes = !in_double_quotes;
        }
        else if (line[i] == '\'' && !in_double_quotes)
        {
            in_single_quotes = !in_single_quotes;
        }

        if (line[i] == '$' && !in_single_quotes && (i == 0 || line[i - 1] != '\\'))
        {
            i += expand_dollar(line + i, new_line + new_line_len) - 1; /* the - 1 is becuase of the increment at the end of the loop */
            new_line_len += strlen(new_line + new_line_len);
        }
        else if (line[i] != '\\' || (i > 0 && line[i - 1] == '\\'))
        {
            new_line[new_line_len] = line[i];
            new_line_len++;
        }
    }
    new_line[new_line_len] = '\0';

    char* thing = malloc(new_line_len + 1);
    strcpy(thing, new_line);
    return thing;
}


