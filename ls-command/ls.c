
#include <dirent.h>
#include "tlpi_hdr.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdlib.h>
#include <libgen.h>

#define PERMISSION_BITS 10

void setPermissions(mode_t mode, char* string)
{
	switch (mode & S_IFMT)
	{
		case S_IFREG:
			string[0] = '-';
			break;
		case S_IFDIR:
			string[0] = 'd';
			break;
		case S_IFLNK:
			string[0] = 'l';
			break;
		case S_IFCHR:
			string[0] = 'c';
			break;
		case S_IFBLK:
			string[0] = 'b';
			break;
		case S_IFSOCK:
			string[0] = 's';
			break;
		case S_IFIFO:
			string[0] = 'f';
			break;
		default:
			string[0] = '-';
			break;
	}
	string[1] = (mode & S_IRUSR) ? 'r' : '-';
	string[2] = (mode & S_IWUSR) ? 'w' : '-';
	string[3] = (mode & S_IXUSR) ? 'x' : '-';
	string[4] = (mode & S_IRGRP) ? 'r' : '-';
	string[5] = (mode & S_IWGRP) ? 'w' : '-';
	string[6] = (mode & S_IXGRP) ? 'x' : '-';
	string[7] = (mode & S_IROTH) ? 'r' : '-';
	string[8] = (mode & S_IWOTH) ? 'w' : '-';
	string[9] = (mode & S_IXOTH) ? 'x' : '-';
	string[10] = '\0';
}

void printFile(char* filename, int displayHidden, int displayInode, int longFormat, int humanReadable)
{
	struct stat st;
	char permissions[PERMISSION_BITS + 1];
	char* oldtime;
	char *newtime;

	lstat(filename, &st);

	if (displayInode)
	{
		printf("%lu ", st.st_ino);
	}
	if (longFormat)
	{
		setPermissions(st.st_mode, permissions);
		printf("%s ", permissions);
		printf("%lu ", st.st_nlink);
		printf("%s ", getpwuid(st.st_uid)->pw_name);
		printf("%s ", getgrgid(st.st_gid)->gr_name);
		if (humanReadable)
		{
			printf("%liKiB ", st.st_size / 1024);
		}
		else
		{
			printf("%li ", st.st_size);
		}
		oldtime = ctime(&st.st_mtim.tv_sec);
		newtime = malloc(strlen(oldtime) + 1);
		strcpy(newtime, oldtime);
		newtime[strlen(newtime) - 9] = '\0';
		printf("%s ", newtime + 4);
		free(newtime);
	}
	printf("%s", basename(filename));
}


void printDir(char* filename, char* seperator, int displayHidden, int displayInode, int longFormat, int humanReadable)
{
	DIR* dirPointer;
	int firstPass = 1;
	struct dirent* dirEntPointer;
	const int bufferSize = 255;
	char buffer[bufferSize + 1];
	int i;
	
	buffer[bufferSize] = '\0';


   dirPointer = opendir(filename);
   if (dirPointer  == NULL)
   {
	fprintf(stderr, "Error opening directory: %s\n", filename);
	exit(EXIT_FAILURE);
   }

   while(1)
   {
	  errno = 0; // make sure we can tell if there's an error
	  dirEntPointer = readdir(dirPointer);
	  if (dirEntPointer == NULL)
		 break;
	  if (displayHidden || dirEntPointer->d_name[0] != '.')
	  {
		if (!firstPass)
			printf("%s", seperator);
		firstPass = 0;
		strcpy(buffer, filename);
		if (buffer[strlen(buffer) - 1] != '/')
		{
			strcat(buffer, "/");
		}
		strcat(buffer, dirEntPointer->d_name);
		  printFile(buffer, displayHidden, displayInode, longFormat, humanReadable);
	  }
   }

   if (closedir(dirPointer) == -1)
   {
	  perror("closedir");
	  exit(EXIT_FAILURE);
   }
}

int main(int argc, char *argv[])
{
	// create pointers
	DIR *dirPointer;
	struct dirent *dirEntPointer;
	struct stat st;
	int i;
	int j;
	int length;
	char* seperator = " ";
	int longFormat = 0;
	int displayHidden = 0;
	int displayInode = 0;
	int filesPassed = 0;
	int humanReadable = 0;

	for (i = 1; i < argc; i++)
	{
		length = strlen(argv[i]);
		if (argv[i][0] == '-' && length > 1) // option argument
		{
			for (j = 1; j < length; j++)
			{
				switch (argv[i][j])
				{
				case 'i':
					displayInode = 1;
					break;
				case 'l':
					longFormat = 1;
					seperator = "\n";
					break;
				case '1': // one per line
					seperator = "\n";
					break;
				case 'a':
					displayHidden = 1;
					break;
				case 'h':
					humanReadable = 1;
					break;
				}
			}
		}
		else // directory or file argument
		{
			filesPassed = 1;
			if (lstat(argv[i], &st) == 0)
			{
				if (st.st_mode & S_IFDIR) // is dir
				{
					printDir(argv[i], seperator, displayHidden, displayInode, longFormat, humanReadable);
				}
				else// if (st.st_mode & S_IFREG)
				{
					printFile(argv[i], displayHidden, displayInode, longFormat, humanReadable);
				}
			}
			else
			{
				fprintf(stderr, "Invalid argument: %s\n", argv[i]);
				return EXIT_FAILURE;
			}
		}
	}

	if (!filesPassed)
	{
		printDir(".", seperator, displayHidden, displayInode, longFormat, humanReadable);
	}

   // check if worked


   // loop until we get NULL from readdir


   // check for errors
   //if (errno != 0)
   //{
	  //perror("readdir");
	  //return EXIT_FAILURE;
   //}



   printf("\n");

	return EXIT_SUCCESS;
}

