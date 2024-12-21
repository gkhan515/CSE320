#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "cookbook.h"

COOKBOOK *cbp;
char *cookbook_name = "rsrc/cookbook.ckb";
char *requested_recipe;
int max_cooks = 1;
RECIPE *main_recipe;
pid_t root;
int chef_tracker[2];

int complete_task (TASK *task);
int wait_for_cook();
int free_cook();

int cook (RECIPE *recipe) {
    RECIPE_LINK *sub = recipe->this_depends_on;

    int num_subs = 0;
    while (sub) {
        num_subs++;

        pid_t p1 = fork(); // TODO: error checking
        if (p1 == 0) {
            return cook(sub->recipe);
        }
        if (!(sub = sub->next)) {
            break;
        }
    }

    for (int i = 0; i < num_subs; i++)
        wait(NULL);

    if (wait_for_cook())
        return 1;

    TASK *current_task = recipe->tasks;
    while (current_task) {
        if (complete_task(current_task))
            return 1;
        current_task = current_task->next;
    }

    if (free_cook())
        return 1;

    close(chef_tracker[0]);
    close(chef_tracker[1]);

    return 0;
}

int complete_task (TASK *task) {
    int fd[2];
    if (pipe(fd) < 0)
        return 1;
    STEP *current_step = task->steps;

    if (task->input_file) {
        int fd = open(task->input_file, O_RDONLY); // TODO: error checking
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (task->output_file) {
        int fd = open(task->output_file, O_CREAT | O_WRONLY); // TODO: error checking
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    while (current_step) {
        pid_t p = fork(); // TODO: error checking
        if (p == 0) { // execute command
            // redirect input/output
            if (!task->input_file && current_step->next)
                dup2(fd[0], STDIN_FILENO);
            if (!task->output_file && current_step->next)
                dup2(fd[1], STDOUT_FILENO);

            close(fd[0]);
            close(fd[1]);
            char *command = *(current_step->words);

            // first, try the util directory
            // then, try PATH
            char *path = malloc(5 + strlen(command) + 1);
            strcpy(path, "util/");
            strcat(path, command);
            if (execvp(path, current_step->words) && execvp(command, current_step->words))
                return 1;
            free(path);
        }
        wait(NULL);
        if (!(current_step = current_step->next))
            break;
    }
    close(fd[0]);
    close(fd[1]);
    return 0;
}

int wait_for_cook () {
    int available_cooks;
    while (1) {
        if (read(chef_tracker[0], &available_cooks, sizeof(int)) < 0) {
            fprintf(stderr, "Error reading from pipe\n");
            return 1;
        }
        if (available_cooks)
            break;
        if (write(chef_tracker[1], &available_cooks, sizeof(int)) < 0) {
            fprintf(stderr, "Error writing to pipe\n");
            return 1;
        }
    }
    available_cooks--;
    if (write(chef_tracker[1], &available_cooks, sizeof(int)) < 0) {
        fprintf(stderr, "Error writing to pipe\n");
        return 1;
    }
    return 0;
}

int free_cook () {
    int available_cooks;
    if (read(chef_tracker[0], &available_cooks, sizeof(int)) < 0) {
        fprintf(stderr, "Error reading from pipe\n");
        return 1;
    }
    available_cooks++;
    if (write(chef_tracker[1], &available_cooks, sizeof(int)) < 0) {
        fprintf(stderr, "Error writing to pipe\n");
        return 1;
    }
    return 0;
}

RECIPE *find_recipe (char *name) {
    RECIPE *cur_recipe = cbp->recipes;
    while (cur_recipe) {
        if (!strcmp(cur_recipe->name, name))
            return cur_recipe;
        cur_recipe = cur_recipe->next;
    }
    return NULL;
}

int validargs (int argc, char *argv[]) {
    if (argc > 6) {
        fprintf(stderr, "Too many arguments\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (!strcmp(arg, "-f")){
            i++;
            arg = argv[i];
            if (!arg) {
                fprintf(stderr, "Error: Cookbook path expected\n");
                return 1;
            }
            cookbook_name = arg;
            continue;
        }
        else if (!strcmp(arg, "-c")) {
            i++;
            arg = argv[i];
            if (!arg) {
                fprintf(stderr, "Error: max_cooks expected\n");
                return 1;
            }
            max_cooks = atoi(arg);
            if (!max_cooks) {
                fprintf(stderr, "-c must be proceeded by a number\n");
                return 1;
            }
            continue;
        }

        requested_recipe = arg;
    }

    return 0;
}