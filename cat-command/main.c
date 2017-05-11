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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>


#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif


int cat_file(char* filename)
{
    char buffer[BUFFER_SIZE];
    int chars_read;
    int chars_written;
    int fd;
    int status;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        exit(1);
    }

    do
    {
        chars_read = read(fd, buffer, BUFFER_SIZE);
        if (chars_read < 0)
        {
            perror("read");
            exit(1);
        }

        chars_written = write(1, buffer, chars_read);
        if (chars_written < 0)
        {
            perror("write");
            exit(1);
        }
    } while (chars_read);

    status = close(fd);
    if (status < 0)
    {
        perror("close");
        exit(1);
    }

    return 0;
}


int main(int argc, char** argv)
{
    int i;
    int status;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file> [file ...]\n", basename(argv[0]));
        return 1;
    }

    for (i = 1; i < argc; i++)
    {
        cat_file(argv[i]);
    }

    return 0;
}

