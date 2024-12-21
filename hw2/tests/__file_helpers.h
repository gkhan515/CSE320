#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int save_stdout;
int save_stderr;

FILE *open_input_file(char *filename);
FILE *open_output_file(char *filename);
void close_output_file(FILE *file);
