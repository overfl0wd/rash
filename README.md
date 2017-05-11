# ra$h

ra$h is a custom *nix shell written in the C programming language. The shell loops until the user types exit, prompting the user for input, forking a new process, and executing the provided command. ra$h includes common Linux features, which have been meticulously rewritten by hand (without invoking the original or similar commands) using system calls and C standard library calls for educational purposes. These are as follows:

- #### groups
    - Lists a user's group membership. This command accepts zero or more arguments specifying usernames. If no argument is provided, the command prints all of the groups of the executing user on the line. If one or more usernames are provided, for each username the command will print the username followed by the list of groups that the user is a member of. 
    - If one of the users is not valid it will print an error message and exit with an exit code.
- #### ls
    - Lists directory contents. Prints the contents of the current directory if no argument is given. Hidden files (those starting with .) are hidden by default. 
    - Handles the argument being a regular file or symbolic link instead of just a directory. In those cases it just prints the name of the file or link. Handles a variable number of arguments (i.e. two or more) that can all be paths to directories, regular files, or symbolic links.
- #### cat
    - Prints the contents of files to stdout. If no arguments are provided an error message is printed and the command exits unsuccessfully. The target argument can be a relative or absolute path to a file, and are processed in the order they are provided.
- #### cp
    - Takes a source file or directory and copies it to the destination. These locations can be relative or absolute paths to files or directories. If source is a directory and the recursive flag hasn't been provided, the command will exit unsuccessfully showing an appropriate error message. Otherwise, the source directory and all of its contents will be recursively copied. 
    - If the destination exists and is a directory then the command will copy source to a file/directory with the same name in the destination directory. If destination exists and is a file then the command should: (a) overwrite that file with source if source is a file, or (b) exit unsuccessfully and print an appropriate error message if source is a directory.
    - If an incorrect number of arguments have been provided then the command will exit unsuccessfully and an appropriate error message will be printed.
    - File permissions are copied from source to destination if destination does not exist.
- #### cd
    - Accepts one argument which is a relative or absolute path to a directory, and then changes the current working directory to that location. If no arguments are provided then the command will change the current working directory to the executing user's home directory. If two or more arguments are provided then all but the first argument will be ignored.
	
## Compiling
Running the makefile in the top-level directory will compile the individual commands as well as the shell itself. The individual commands will be placed in the custom bin directory which the shell points to.

#### Dependencies
 - make
     - ```sudo apt-get install make```
 - gcc
     - ```sudo apt-get install gcc```
 - libreadline-dev
     - ```sudo apt-get install libreadline-dev```
	
## Usage

Executing the name of the compiled shell, "rash", will change the current prompt to the rash prompt. 
```
:~$ ./shell-command/rash
```
If the given command is groups, ls, cat, or cp, the shell will run the developers' custom version of these commands. If the given command is anything else, and valid, it will execute the distribution's included version of that command. If the given command does not exist on the system, the last bit of the prompt will turn red, indicating an unsuccessful lookup of the given command.

If any of the system calls fail (e.g. if the user does not have appropriate permissions) then the command exits unsuccessfully and an appropriate error message will be printed. 

The built-in commands allow receiving options either grouped or separate and in any order relative to the arguments, e.g. the following should all produce the same:
```
ls /home -l -a
ls /home -la
ls -la /home
ls -l /home -a
```

#### Arguments
 - ##### ls
    - **-a** Prints hidden files (those starting with .)
    - **-i** Print the inode index number of each file
    - **-l** Print the long listing format
    - **-1** Print one entry per line (basically replace the spaces with new lines)
    - **-h** Human readable (raw bytes to megabytes, etc)
    - **-t** Sort by modification time (newest first)
    - **-r** Reverse time sort order (oldest
 - ##### cp
    - **-r** Recursively copy the target directory

## To-do
- [ ] Learn why the shell crashes when hitting enter with no input provided, and fix.  
- [ ] Add more optional arguments to the commands.

## History

- November 6th, 2016 - groups command finished
- November 22nd, 2016 - ls command finished
- December 9th, 2016 - cp, cat, shell finished
- May 10th, 2017 - finally got around to uploading this

## Credits

This project was created for *CSI-230, Linux / Unix Programming* at Champlain College in Burlington, VT by the following:

- Myself, Dana Taylor
- [Nick Boyle](https://github.com/nickeb96)
- [Michael Dargie](https://github.com/MikeDargie) 

## License
See the included LICENSE file. GNU GPL v3.0 applies.
