#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cookbook.h"
#include "cook.h"

int main(int argc, char *argv[]) {
    int err = 0;
    FILE *in;
    if (validargs(argc, argv))
        exit(1);

    if((in = fopen(cookbook_name, "r")) == NULL) {
    	fprintf(stderr, "Can't open cookbook '%s': %s\n", cookbook_name, strerror(errno));
    	exit(1);
    }
    cbp = parse_cookbook(in, &err);
    if(err) {
    	fprintf(stderr, "Error parsing cookbook '%s'\n", cookbook_name);
    	exit(1);
    }
    // unparse_cookbook(cbp, stdout);
    if (!requested_recipe)
        main_recipe = cbp->recipes;
    else if (!(main_recipe = find_recipe(requested_recipe))) {
        fprintf(stderr, "No such recipe: %s\n", requested_recipe);
        exit(1);
    }

    root = getpid();
    if (pipe(chef_tracker)) {
        fprintf(stderr, "Error opening pipe\n");
        exit(1);
    }
    if (write(chef_tracker[1], &max_cooks, sizeof(int)) < 0){
        fprintf(stderr, "Error writing to pipe\n");
        exit(1);
    }

    int ret = cook(main_recipe);

    exit(ret);
}


