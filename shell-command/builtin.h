

#ifndef BUILTIN_H_
#define BUILTIN_H_


int builtin_exit(int argc, char** argv);

int builtin_cd(int argc, char** argv);

int builtin_export(int argc, char** argv);

int builtin_unset(int argc, char** argv);

int builtin_true(int argc, char** argv);

int builtin_false(int argc, char** argv);

#endif

