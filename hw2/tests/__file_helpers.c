#include "__file_helpers.h"

#include <sys/resource.h>
#include <errno.h>

FILE *open_input_file(char *filename) {
    FILE *f = freopen(filename, "r", stdin);
    if(f == NULL) {
        fprintf(stderr, "Couldn't open file: %s, ERRNO: %s", filename, strerror(errno));
	abort();
    }
    return f;
}

FILE *open_output_file(char *filename) {
    struct rlimit rl = {.rlim_cur = 1000000, .rlim_max = 1000000};
    int ret = setrlimit(RLIMIT_FSIZE, &rl);
    if(ret == -1)
      perror("setrlimit");
    save_stdout = dup(1);
    save_stderr = dup(2);
    FILE *f = freopen(filename, "w", stdout);
    if(f == NULL) {
        fprintf(stderr, "Couldn't open file: %s, ERRNO: %s", filename, strerror(errno));
	abort();
    }
    return f;
}

void close_output_file(FILE *file) {
    fclose(file);
    dup2(save_stdout, 1);
    dup2(save_stderr, 2);
    close(save_stdout);
    close(save_stderr);
}
