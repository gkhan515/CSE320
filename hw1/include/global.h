/*
 * DO NOT MODIFY THE CONTENTS OF THIS FILE.
 * IT WILL BE REPLACED DURING GRADING
 */
#ifndef CONST_H
#define CONST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <utime.h>
#include <dirent.h>
#include <sys/stat.h>

#define USAGE(program_name, retcode) do { \
fprintf(stderr, "USAGE: %s %s\n", program_name, \
"[-h] -s|-d [-c] [-p DIR]\n" \
"   -h       Help: displays this help menu.\n" \
"   -s       Serialize: traverse tree of files, output serialized data.\n" \
"   -d       Deserialize: read serialized data, reconstruct tree of files.\n" \
"            Optional additional parameter for both -s and -d:\n" \
"               -p DIR       DIR is a pathname that specifies the source directory\n" \
"                            for serialization or the target directory for deserialization.\n" \
"                            If this parameter is not present, the pathname `.`\n" \
"                            (referring to the current working directory) is assumed.\n" \
"            Optional additional parameter for -d:\n" \
"               -c           ``clobber'': the program will overwrite existing files,\n" \
"                            rather than terminating with an error, and it will ignore\n" \
"                            errors that result when attempts is made to create directories\n" \
"                            that already exist.\n"); \
exit(retcode); \
} while(0)

/* Options info, set by validargs. */
int global_options;

/*
 * The following variables have been provided for you.
 * You MUST use them for their stated purposes, because you are not permitted
 * to declare any arrays (or use any array brackets at all) in your own code.
 */

/*
 * Buffer to hold a pathname component read from stdin.
 */
char name_buf[NAME_MAX];

/*
 * Buffer to hold the pathname of the current file or directory,
 * including the null byte terminator.  The path could be an absolute path
 * (starting with '/') or a relative path (not starting with '/').
 * relative path.
 */
char path_buf[PATH_MAX];

/*
 * Current length of the path in path_buf, not including the terminating
 * null byte.
 */
int path_length;

/*
 * The following are functions that you are required to implement.
 * Refer to the assignment handout and the documentation associated with the
 * stubs in transplant.c for detailed specification.
 */

int path_init(char *name);
int path_push(char *name);
int path_pop();
int deserialize_directory(int depth);
int deserialize_file(int depth);
int serialize_directory(int depth);
int serialize_file(int depth, off_t size);
int serialize();
int deserialize();

int validargs(int argc, char **argv);

#endif
