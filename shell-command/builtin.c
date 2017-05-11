
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/wait.h>


int builtin_exit(int argc, char** argv)
{
    int exit_status = 0;

    if (argc > 1)
    {
        exit_status = atoi(argv[1]);
    }

    return exit_status;
}

int builtin_cd(int argc, char** argv)
{
    int exit_status;
    char* dir;
    char str[4096];

    if (argc > 1)
    {
        dir = argv[1];
    }
    else
    {
        dir = getenv("HOME");
    }

    exit_status = chdir(dir);

    getcwd(str, 4095);
    setenv("PWD", str, 1);

    return exit_status;
}


int builtin_export(int argc, char** argv)
{
    int exit_status = 0;
    char* str;

    if (argc > 1)
    {
        str = malloc(strlen(argv[1]) + 1);
        strcpy(str, argv[1]);
        exit_status = putenv(str);
    }
    else
    {
        exit_status = 1;
    }

    return exit_status;
}

int builtin_unset(int argc, char** argv)
{
    int exit_status = 0;

    if (argc > 1)
    {
        exit_status = unsetenv(argv[1]);
    }
    else
    {
        exit_status = 1;
    }

    return exit_status;

}

int builtin_true(int argc, char** argv)
{
    return 0;
}

int builtin_false(int argc, char** argv)
{
    return 1;
}
