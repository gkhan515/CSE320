#ifndef COOK_H
#define COOK_H

#include <unistd.h>
#include "cookbook.h"

extern COOKBOOK *cbp;
extern char *cookbook_name;
extern char *requested_recipe;
extern int max_cooks;
extern RECIPE *main_recipe;
extern pid_t root;
extern int chef_tracker[2];

int validargs (int argc, char *argv[]);
int cook (RECIPE *recipe);
RECIPE *find_recipe (char *name);

#endif