
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <libgen.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif


int copy_file(char* src, char* dst)
{
    int src_fd;
    int dst_fd;
    char buffer[BUFFER_SIZE];
    int chars_read;
    int chars_written;
    struct stat src_stat;


    src_fd = open(src, O_RDONLY);
    if (src_fd < 0)
    {
        perror("open");
        exit(1);
    }


    if (access(dst, F_OK) == 0)
    {
        dst_fd = open(dst, O_WRONLY | O_TRUNC);
    }
    else
    {
        if (stat(src, &src_stat) == 0)
        {
            dst_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
        }
        else
        {
            perror("stat");
            exit(1);
        }
    }

    if (dst_fd < 0)
    {
        perror("open");
        exit(1);
    }


    do
    {
        chars_read = read(src_fd, buffer, BUFFER_SIZE);
        if (chars_read < 0)
        {
            perror("read");
            exit(1);
        }

        chars_written = write(dst_fd, buffer, chars_read);
        if (chars_written < 0)
        {
            perror("write");
            exit(1);
        }
    } while (chars_read);


    if (close(src_fd) < 0)
    {
        perror("close");
        exit(1);
    }

    if (close(dst_fd) < 0)
    {
        perror("close");
        exit(1);
    }


    return 0;
}


int copy_dir(char* src, char* dst, int depth)
{
    DIR* dir;
    struct dirent* de;
    struct stat src_st;
    char src_path[4096];
    char dst_path[4096];


    if (depth > 3)
    {
        fprintf(stderr, "too deep\n");
        return 0;
    }

    if (access(dst, F_OK) != 0)
    {
        if (stat(src, &src_st) == 0)
        {
            if (mkdir(dst, src_st.st_mode))
            {
                perror("mkdir");
                exit(1);
            }
        }
    }

    dir = opendir(src);

    while ((de = readdir(dir)))
    {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        {
            continue;
        }


        strcpy(src_path, src);
        strcat(src_path, "/");
        strcat(src_path, de->d_name);
        stat(src_path, &src_st);

        strcpy(dst_path, dst);
        strcat(dst_path, "/");
        strcat(dst_path, de->d_name);


        if (S_ISDIR(src_st.st_mode))
        {
            copy_dir(src_path, dst_path, depth + 1);
        }
        else
        {
            copy_file(src_path, dst_path);
        }
    }

    closedir(dir);

    return 0;
}


int main(int argc, char** argv)
{
    char* src_arg;
    char* dst_arg;
    char dst_path[4096];
    struct stat src_st;
    struct stat dst_st;
    int r_flag;
    int old_mask;
    int i;


    src_arg = 0;
    dst_arg = 0;
    old_mask = umask(0);
    r_flag = 0;


    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-r") == 0)
        {
            r_flag = 1;
        }
        else
        {
            if (!src_arg)
            {
                src_arg = argv[i];
            }
            else if (!dst_arg)
            {
                dst_arg = argv[i];
            }
        }
    }

    if (!src_arg || !dst_arg)
    {
        fprintf(stderr, "Usage: %s [-r] <source> <destination>\n", basename(argv[0]));
        return 1;
    }

    strcpy(dst_path, dst_arg);

    if (stat(src_arg, &src_st))
    {
        perror("stat");
        return 1;
    }

    if (access(dst_path, F_OK) == 0)
    {
        if (stat(dst_arg, &dst_st))
        {
            perror("stat");
            return 1;
        }
        if (S_ISDIR(dst_st.st_mode))
        {
            strcat(dst_path, "/");
            strcat(dst_path, basename(src_arg));
        }
    }

    if (S_ISDIR(src_st.st_mode))
    {
        if (!r_flag)
        {
            fprintf(stderr, "%s is a directory\n", src_arg);
            return 1;
        }
        else
        {
            copy_dir(src_arg, dst_path, 0);
        }
    }
    else
    {
        copy_file(src_arg, dst_path);
    }

    umask(old_mask);

    return 0;
}

