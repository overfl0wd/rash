
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>

void printListOfGroups(char* userName);
int isUserInGroup(char* userName, struct group* g);


int main( int argc, char *argv[] )
{
  char *user;
  int i;
  int userExists = 0;

  if (argc == 1)
  {
    printListOfGroups(getlogin());
  }
  else
  {
	  for (i = 1; i < argc; i++)
	  {
		  struct passwd* p;
		  setpwent();
		  while (p = getpwent())
		  {
		  	if (strcmp(p->pw_name, argv[i]) == 0)
		  	{
		  		userExists = 1;
		  		break;
		  	}
		  }
		  endpwent();

		  if (!userExists)// user does not exist
		  {
			  fprintf(stderr, "groups: '%s': no such user\n", argv[i]);
			  return 1;
		  }
		  printf("%s : ", argv[i]);
		  printListOfGroups(argv[i]);
		  userExists = 0;
	  }
  }

	return 0;
}


void printListOfGroups(char* userName)
{
  struct group* g;
  struct passwd* p;

  setpwent();
  while (p = getpwent())
  {
	  if (strcmp(p->pw_name, userName) == 0)
	  {
		  printf("%s ", getgrgid(p->pw_gid)->gr_name);
		  break;
	  }
  }
  endpwent();

  setgrent();
  while (g = getgrent())
  {
 	if (isUserInGroup(userName, g))
    {
 	  printf("%s ", g->gr_name);
    }
  }
  printf("\n");
  endgrent();
}


int isUserInGroup(char* userName, struct group* g)
{
  int i;
  for (i = 0; g->gr_mem[i]; i++)
  {
    if (strcmp(g->gr_mem[i], userName) == 0)
    {
      return 1;
    }
  }
  return 0;
}
