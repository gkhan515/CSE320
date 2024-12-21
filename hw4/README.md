# Homework 4 Parallel 'Cook' - CSE 320 - Fall 2024
#### Professors Eugene Stark and Dongyoon Lee

### **Due Date: Friday 11/22/2024 @ 11:59pm**

## Introduction

The goal of this assignment is to become familiar with low-level Unix/POSIX system
calls related to processes, signal handling, files, and I/O redirection.
You will implement a program, called `cook`, that behaves like a simplified version
of `make`, with the ability to run jobs in parallel up to a specified maximum.

### Takeaways

After completing this assignment, you should:

* Understand process execution: forking, executing, and reaping.
* Understand signal handling.
* Understand the use of "dup" to perform I/O redirection.
* Have a more advanced understanding of Unix commands and the command line.
* Have gained experience with C libraries and system calls.
* Have enhanced your C programming abilities.

## Hints and Tips

* We **strongly recommend** that you check the return codes of **all** system calls
  and library functions.  This will help you catch errors.
* **BEAT UP YOUR OWN CODE!** Use a "monkey at a typewriter" approach to testing it
  and make sure that no sequence of operations, no matter how ridiculous it may
  seem, can crash the program.
* Your code should **NEVER** crash, and we will deduct points every time your
  program crashes during grading.  Especially make sure that you have avoided
  race conditions involving process termination and reaping that might result
  in "flaky" behavior.  If you notice odd behavior you don't understand:
  **INVESTIGATE**.
* You should use the `debug` macro provided to you in the base code.
  That way, when your program is compiled without `-DDEBUG`, all of your debugging
  output will vanish, preventing you from losing points due to superfluous output.

> :nerd: When writing your program, try to comment as much as possible and stay
> consistent with code formatting.  Keep your code organized, and don't be afraid
> to introduce new source files if/when appropriate.

### Reading Man Pages

This assignment will involve the use of many system calls and library functions
that you probably haven't used before.
As such, it is imperative that you become comfortable looking up function
specifications using the `man` command.

The `man` command stands for "manual" and takes the name of a function or command
(programs) as an argument.
For example, if I didn't know how the `fork(2)` system call worked, I would type
`man fork` into my terminal.
This would bring up the manual for the `fork(2)` system call.

> :nerd: Navigating through a man page once it is open can be weird if you're not
> familiar with these types of applications.
> To scroll up and down a line at a time, you simply use the **up arrow key** and
> **down arrow key** or **j** and **k**, respectively.  Scrolling a page at a time
> is done with the space bar and **b**.  To exit the page, simply type **q**.
> That having been said, long `man` pages may look like a wall of text.
> So it's useful to be able to search through a page.
> This can be done by typing the **/** key, followed by your search phrase,
> and then hitting **enter**.
> Note that man pages are displayed with a program known as `less`.
> For more information about navigating the `man` pages with `less`,
> run `man less` in your terminal.

Now, you may have noticed the `2` in `fork(2)`.
This indicates the section in which the `man` page for `fork(2)` resides.
Here is a list of the `man` page sections and what they are for.

| Section          | Contents                                |
| ----------------:|:--------------------------------------- |
| 1                | User Commands (Programs)                |
| 2                | System Calls                            |
| 3                | C Library Functions                     |
| 4                | Devices and Special Files               |
| 5                | File Formats and Conventions            |
| 6                | Games et. al                            |
| 7                | Miscellanea                             |
| 8                | System Administration Tools and Daemons |

From the table above, we can see that `fork(2)` belongs to the system call section
of the `man` pages.
This is important because there are functions like `printf` which have multiple
entries in different sections of the `man` pages.
If you type `man printf` into your terminal, the `man` program will start looking
for that name starting from section 1.
If it can't find it, it'll go to section 2, then section 3 and so on.
However, there is actually a Bash user command called `printf`, so instead of getting
the `man` page for the `printf(3)` function which is located in `stdio.h`,
we get the `man` page for the Bash user command `printf(1)`.
If you specifically wanted the function from section 3 of the `man` pages,
you would enter `man 3 printf` into your terminal.

> :scream: Remember this: **`man` pages are your bread and butter**.
> Without them, you will have a very difficult time with this assignment.

## Getting Started

Fetch and merge the base code for `hw4` as described in `hw0`.
You can find it at this link: https://gitlab02.cs.stonybrook.edu/cse320/hw4

Here is the structure of the base code:
<pre>
.
├── .gitlab-ci.yml
└── hw4
    ├── demo
    │   ├── cook
    │   └── cook_db
    ├── hw4.sublime-project
    ├── include
    │   ├── cookbook.h
    │   └── debug.h
    ├── lib
    │   ├── cookbook_parser.c
    │   └── cookbook_parser.o
    ├── Makefile
    ├── rsrc
    │   ├── cookbook.ckb -> eggs_benedict.ckb
    │   ├── eggs_benedict.ckb
    │   └── hello_world.ckb
    ├── src
    │   └── main.c
    ├── tests
    │   ├── base_tests.c
    │   ├── rsrc
    │   │   └── hello_world.out
    │   └── test_cook.py
    └── util
        ├── add -> generic_step
        ├── assemble -> generic_step
        ├── beat -> generic_step
        ├── boil -> generic_step
        ├── buy -> generic_step
        ├── churn -> generic_step
        ├── cook -> generic_step
        ├── generic_step
        ├── generic_step.c
        ├── melt -> generic_step
        ├── milk -> generic_step
        ├── pour_on -> generic_step
        ├── reduce -> generic_step
        ├── remove -> generic_step
        ├── separate -> generic_step
        ├── serve -> generic_step
        └── skim -> generic_step
</pre>
The contents of these files are discussed further below.

If you run `make`, the code should compile correctly, resulting in an
executable `bin/cook`.  If you run this program, it should read in a
demonstration "cookbook" file and print out what it has read.
This is just to demonstrate the use of the "cookbook parser" library
that I have provided.  The actual functionality that `cook` is to have
is described below, and you have to implement it!

## `Cook`: Overview

Suppose you are hosting an elaborate dinner, and you need to prepare the
dishes to be served.  You have a cookbook containing recipes that you want
to follow.  As you are probably aware, in a real cookbook the recipes are
not independent of one another, but rather quite often one recipe will depend
on one or more "sub-recipes".  For example, if you are cooking Eggs Benedict,
you would follow one sub-recipe for hollandaise sauce and another for poached eggs.
If you are really doing things "from scratch", before you could make the
hollandaise sauce you might first have to go milk a cow and churn the butter.
So, once you have decided you want to make Eggs Benedict, you will look at
the Eggs Benedict recipe and see that it requires that you also carry out
the recipes for hollandaise sauce and for poached eggs.  When you look at the
recipe for hollandaise sauce you see that it requires butter, so you also
have carry out a sub-recipe for churning butter.

Now suppose you have some friends helping you.  Each recipe breaks down into
a number of tasks to be performed, and it stands to reason that if each of your
friends takes on a different task, you might be able to get finished faster than
if you did all the tasks yourself, one at a time.  However, there are constraints
on the order in which the tasks can be performed; for example, you cannot start
the task of cooking the hollandaise sauce until you have churned the butter.
As the chef, you are faced with the problem of identifying all the tasks to
be performed and the dependencies between them, and of assigning tasks to your
friends in order to keep all of them as busy as possible.

The same type of problem occurs in building a distribution bundle for a software
product.  Often this will require the building of sub-components such as libraries,
documentation bundles, etc., and these sub-components might have sub-sub-components,
and so on.  The build program `make` addresses this problem.  It determines the
dependencies between the various components and sub-components and schedules the
builds so as to ultimately achieve the goal of creating the final product.
The `make` program obtains information about the various components to be built,
their dependencies, and how to build them from a `Makefile`.  Modern versions of
`make` are able to schedule various tasks in parallel; thereby taking fuller advantage
of the computer system's facilities such as multiple CPUs, disk arrays, and so on to
complete a build as fast as possible.

For this assignment you will be writing a program that performs a function similar
to that of `make`.  In order to focus in on the core problem of scheduling and
managing a collection of concurrent tasks, we will abstract away from a lot of
the bells and whistles that are found in the real `make` program.
To make it easier to give a more concrete description of what your program for this
assignment is supposed to accomplish, we will continue using the cooking metaphor
as a source of terminology.  The program you are to implement is called `cook`,
and its purpose is to schedule and carry out the series of steps required in order
to complete a recipe, such as Eggs Benedict.  As the source of information about what
to do, the `cook` program uses a "cookbook", which is a text file that lists
information about a collection of recipes.  The cookbook file used by `cook` has
a syntax that is a simplified version of the syntax of a `Makefile`.
Here is an example of what a cookbook file looks like:

```
eggs_benedict: hollandaise_sauce poached_eggs english_muffin canadian_bacon
  assemble muffin eggs bacon
  pour_on sauce
  serve guests

hollandaise_sauce: butter egg_yolks lime_juice cream salt pepper
  melt butter
  beat yolks
  add juice cream salt pepper
  cook mixture
  
poached_eggs: eggs
  boil water | reduce heat to simmer | cook eggs | remove eggs from water

english_muffin: get_gas
  buy muffins from store

canadian_bacon: get_gas
  buy bacon from store

egg_yolks: eggs
  mkdir -p tmp
  separate yolks from eggs > tmp/yolks

eggs: get_gas
  buy eggs from store

butter: cream
  churn cream into butter

lime_juice:
  buy juice from store

cream:
  milk cow
  skim cream from milk

salt:
  buy salt from store

pepper:
  buy pepper from store

get_gas:
```

The file consists of a sequence of *recipes*.
The first line of each recipe states the name of the recipe,
then a colon (':') followed by a list of names of sub-recipes on which
this recipe depends.
The lines following this header line describe a series of *tasks* that
must be performed in order to complete this recipe.
A recipe is terminated by a blank line.

In order to `cook` a recipe, first all of the sub-recipes must be cooked.
Once that has been done, the tasks listed in the recipe must be carried
out in sequence.
Each task in a cookbook is actually a command to be carried out by executing
a program with arguments.  The commands have a syntax that is a simplified
version of the syntax understood by a Linux command shell.  Each command
consists of a sequence of *steps* separated by a vertical bar '|' character,
and optionally followed by an *input redirection* of the form  `< infile`,
or an *output redirection* of the form `> outfile`, or both.
Each step consists of a sequence of *words*, the first of which is the name
of program to be executed and the remainder of which are arguments.
Most of the tasks in the example above consist of only one step and they
do not have any redirections.
The task for recipe `poached_eggs` contains multiple steps.
Such a task is executed as a *pipeline*: the steps are carried out by concurrently
executing processes, one process for each step, and with the standard output
of each process *piped* as the standard input to the next process in the pipeline.
If there is an input redirection, then the standard input of the first process
in the pipeline will come from the specified input file.
Similarly, if there is an output redirection, then the standard output of the
last process in the pipeline will be sent to the specified output file,
which is initially created if it did not exist and truncated if it did exist.
The task for recipe `egg_yolks` illustrates the use of output redirection.

In order to try to get the cooking done faster, the `cook` program will
carry out the tasks of multiple recipes in parallel, up to a maximum number of
"cooks" which is specified as a command-line argument to `cook` when it is started.
However, the tasks cannot be carried out in just any old order; rather they must
satisfy the sequencing constraints implied by the cookbook.
In particular, the individual tasks in a single recipe must be done in sequence,
and the task list for a recipe cannot be started until all of the sub-recipes
on which it depends have been completed.
Within these constraints, `cook` will try to keep as many cooks busy as possible.
This means that `cook` will begin processing a recipe as soon as all of its
sub-recipes have been completed and the number of recipes currently being
processed is less than the specified maximum number (of "cooks").

To make things more concrete, below is a transcript created by using
`cook` to cook eggs benedict with the maximum number of cooks set to one.

```
START   [1634749750.808,543355, 4] buy pepper from store
END     [1634749751.209,543355, 4]
START   [1634749751.210,543358, 1] buy salt from store
END     [1634749751.310,543358, 1]
START   [1634749751.311,543361, 0] buy juice from store
END     [1634749751.311,543361, 0]
START   [1634749751.312,543365, 2] buy muffins from store
END     [1634749751.512,543365, 2]
START   [1634749751.513,543368, 9] buy bacon from store
END     [1634749752.413,543368, 9]
START   [1634749752.414,543371, 7] buy eggs from store
END     [1634749753.114,543371, 7]
START   [1634749753.115,543374, 3] boil water
START   [1634749753.115,543375, 1] reduce heat to simmer
START   [1634749753.116,543376, 1] cook eggs
START   [1634749753.116,543377, 3] remove eggs from water
END     [1634749753.215,543375, 1]
END     [1634749753.216,543376, 1]
END     [1634749753.415,543374, 3]
END     [1634749753.416,543377, 3]
START   [1634749753.417,543380, 6] separate yolks from eggs
END     [1634749754.017,543380, 6]
START   [1634749754.018,543383, 8] milk cow
END     [1634749754.818,543383, 8]
START   [1634749754.819,543385, 1] skim cream from milk
END     [1634749754.919,543385, 1]
START   [1634749754.920,543388, 1] churn cream into butter
END     [1634749755.020,543388, 1]
START   [1634749755.022,543391, 1] melt butter
END     [1634749755.122,543391, 1]
START   [1634749755.122,543393, 1] beat yolks
END     [1634749755.223,543393, 1]
START   [1634749755.223,543395, 0] add juice cream salt pepper
END     [1634749755.224,543395, 0]
START   [1634749755.224,543397, 7] cook mixture
END     [1634749755.924,543397, 7]
START   [1634749755.925,543400, 8] assemble muffin eggs bacon
END     [1634749756.725,543400, 8]
START   [1634749756.726,543402, 2] pour_on sauce
END     [1634749756.926,543402, 2]
START   [1634749756.927,543404, 6] serve guests
END     [1634749757.527,543404, 6]
```

Each line of the transcript shows the start or end of a particular step in a recipe.
The first number within the square brackets represents the time (`seconds.msec`)
at which the step was started, the second number is the process ID of the process
performing the step, and the third number is the minimum time (in units of tenths
of a second) that step will take.  The `START` line for each step additionally shows
the command and arguments for that step.

You might suppose that the transcript above was output by the `cook` program itself,
but actually that is not the case.  To create this transcript, I made executables named
`buy`, `boil`, `reduce`, `remove`, `cook`, `separate`, `milk`, `skim`, `churn`, `melt`,
`beat`, `add`, `cook`, `assemble`, `pour_on`, and `serve`, for the steps having these
command names as their first word.  When these programs were run, they each printed out
two lines of the above transcript:  a `START` line when they started and an `END` line
when they finished.  The `cook` program itself ran silently (as your implementation should).

  > :nerd: Actually, I didn't write a separate demonstration program for each of the
  > "commands" above -- I just created one executable, called `generic_step` and I created
  > a number of links to that same executable.  When the `generic_step` program is run,
  > it announces itself based on the name by which it was invoked (*i.e.* `argv[0]`)
  > together with the other information.  The `generic_step` program chooses a random
  > number of seconds from 0 to 9 and delays for that amount of time between printing
  > the `START` line and printing the `END` line, to simulate the time it would take to
  > carry out the step.

Note that the steps in the above transcript were carried out in sequence, the `END`
of each step occurring before the `START` of the next, except for the steps involved
in the one task in the `poached_eggs` recipe.  That is because these steps occur as part
of a *pipeline*, which consists of a sequence concurrent processes that pass data
from one to the next.  More explanation about pipelines is given below.

Now suppose we ask `cook` to carry out the Eggs Benedict recipe using a maximum of
ten cooks, rather than one.  The result achieved in one particular run is the following:

```
START   [1634749868.983,543437, 4] milk cow
START   [1634749868.983,543434, 4] buy juice from store
START   [1634749868.984,543444, 3] buy muffins from store
START   [1634749868.984,543433, 2] buy pepper from store
START   [1634749868.984,543443, 4] buy bacon from store
START   [1634749868.984,543432, 9] buy salt from store
START   [1634749868.985,543445, 2] buy eggs from store
END     [1634749869.184,543433, 2]
END     [1634749869.185,543445, 2]
START   [1634749869.186,543450, 6] boil water
START   [1634749869.186,543452, 6] reduce heat to simmer
START   [1634749869.187,543451, 7] separate yolks from eggs
START   [1634749869.187,543453, 4] cook eggs
START   [1634749869.187,543454, 3] remove eggs from water
END     [1634749869.284,543444, 3]
END     [1634749869.384,543437, 4]
END     [1634749869.384,543434, 4]
END     [1634749869.384,543443, 4]
START   [1634749869.384,543456, 5] skim cream from milk
END     [1634749869.487,543454, 3]
END     [1634749869.587,543453, 4]
END     [1634749869.786,543450, 6]
END     [1634749869.786,543452, 6]
END     [1634749869.885,543456, 5]
END     [1634749869.885,543432, 9]
START   [1634749869.886,543459, 8] churn cream into butter
END     [1634749869.887,543451, 7]
END     [1634749870.686,543459, 8]
START   [1634749870.687,543463, 8] melt butter
END     [1634749871.487,543463, 8]
START   [1634749871.488,543465, 9] beat yolks
END     [1634749872.388,543465, 9]
START   [1634749872.389,543467, 3] add juice cream salt pepper
END     [1634749872.689,543467, 3]
START   [1634749872.690,543469, 9] cook mixture
END     [1634749873.590,543469, 9]
START   [1634749873.591,543472, 8] assemble muffin eggs bacon
END     [1634749874.391,543472, 8]
START   [1634749874.392,543474, 1] pour_on sauce
END     [1634749874.492,543474, 1]
START   [1634749874.493,543476, 7] serve guests
END     [1634749875.193,543476, 7]
```

If you look closely, you will see that this schedule shows steps executing
in parallel.  For example, the initial steps of buying things from the store
were carried out concurrently by several "cooks", rather than sequentially
as in the earlier schedule.  You might find it useful to draw a diagram
(like those in the course textbook) that depicts the steps as "concurrent flows".
See if you can determine the maximum number of "cooks" that were ever active
at once in this schedule.  Was it 10?  Why not?

## `Cook`: Specification and Sketch of Implementation

Your assignment is to implement the `cook` program to carry out steps in recipes
as outlined above.  Your program should accept a command line as follows:

```
cook [-f cookbook] [-c max_cooks] [main_recipe_name]
```

The optional argument `-f cookbook` is used to specify the name of a file that
contains the cookbook.  If omitted, the filename `cookbook.ckb` (in the current
working directory) is used as the default.
The optional argument `-c max_cooks` is used to specify the number of "cooks".
If this argument is given, then `cook` should attempt to have as many cooks busy
actively carrying out a recipe as possible, up to a maximum of `max_cooks` cooks.
If this argument is not given, then the maximum number of cooks should be limited to one,
which means that recipes must be performed sequentially
(except for steps in a single pipeline, as discussed above).
The optional final argument `main_recipe_name`, if given, is interpreted as the name
of the "main recipe" to be prepared.  If this argument is omitted, then it is
assumed that the first recipe in the cookbook is the main recipe.
If a main recipe is specified, but no recipe with that name exists in the cookbook,
then it is an error and `cook` should exit with status `EXIT_FAILURE`.
Similarly, if any other error occurs, so that the main recipe is not completed
successfully, then `cook` should exit with status `EXIT_FAILURE`.  If the main recipe
is completed successfully, then `cook` should terminate with exit status `EXIT_SUCCESS`.

When `cook` starts, the first thing it does is to read and parse the cookbook file.
Although being able to write code to parse files written in a simple syntax is an
important and useful skill to have, it is not what I want this assignment to focus on.
From previous experience I have found that if I were to ask you to implement the
cookbook parsing yourself, many of you would get stuck at this point and never
get on to the core of the assignment that is related to the topics of this course.
So, I have written the cookbook parsing code for you (more about that later on).
The result of parsing the cookbook is a data structure that represents the recipes,
tasks, and steps.  In brief, a cookbook consists of a list of recipes,
a recipe comprises a list of sub-recipes and a list of tasks, a task consists of a
list of steps and possibly an input and/or output file, and a step consists of a list
of words.  The C definitions used to represent these data structures are given later.

Once the cookbook has been parsed, `cook` performs an analysis, starting with the
"main recipe", to determine all of the sub-recipes required by the main recipe,
all the sub-sub-recipes required by the sub-recipes, and so on.
The result of this analysis is the subset of recipes in the cookbook that are ultimately
required to complete the main recipe, and in addition the subset of the set of required
recipes that are "leaves" in the sense that they do not themselves depend on any further
sub-recipes.  The analysis is performed via a recursive traversal of the cookbook,
starting from the specified main recipe.

Once the dependency analysis is complete, `cook` then begins processing recipes.
The processing makes use of a *work queue*, which at any given time will contain those
recipes that are (1) required for the main recipe; (2) are ready to have their task
list processed because all of the sub-recipes on which they depend have been completed;
and (3) have not yet had processing begin on their task list.
Initially the work queue will be populated only by the leaf recipes that were identified
during the analysis phase.

The "main processing loop" executed by `cook` does the following:

 * If the work queue is empty, and there are no recipes actively being processed,
   then cooking is complete and the processing loop terminates.  `Cook` exits
   with an exit status that reflects whether or not the main recipe has been
   successfully completed.

 * If the work queue is nonempty, but the number of recipes actively being processed
   is equal to the specified maximum number of cooks, then `cook` waits
   (using the `sigsuspend()` system call) for the completion of one or more of the
   recipes that are currently being processed.
 
 * If the work queue is nonempty and the number of recipes actively being processed
   is less than the specified maximum number of cooks, then `cook` removes the
   first recipe from the work queue and starts (using `fork()`) a "cook process"
   to do the processing the task list in that recipe.

When a cook process is started for a recipe, it carries out the tasks for that recipe
in sequence.  For each task, the cook uses `fork()` to create child processes to
run each of the steps in that task.  These child processes run concurrently,
with the standard output of each process in the pipeline "piped" to the standard input
of the next process in the pipeline.  In addition, if the task has an input redirection,
then the standard input of the first process in the pipeline is redirected from the
specified input file and if the task has an output redirection, then the standard output
of the last process in the pipeline is redirected to the specified output file.
An input file specified in an input redirection must already exist.
An output file specified in an output redirection need not exist: if it does not it
is created and if it does exist it is truncated to zero length.
Besides the `fork()` system call used to create the processes, the creation of the pipeline
will involve the use of the `open()`, `pipe()`, and `dup2()` system calls to set up the pipes
and redirections, and the `execvp()` system call must be used to execute the individual steps.
Note that tasks in a cookbook are expressed in a simplified version of the syntax
understood by a shell such as `bash`, and what a cook process does in setting up a
pipeline to perform a task is exactly what a shell would do if it were given
the task as a command.

  > **Important:**  You **must** create the processes in a pipeline using calls to
  > `fork()` and `execvp()`.  You **must not** use the `system()` function, nor use any
  > form of shell in order to create the pipeline, as the purpose of the assignment is
  > to give you experience with using the system calls involved in doing this.

  > **Note:**  When `cook` program executes a step in a command, it should first attempt
  > to execute the command from the `util/` subdirectory of the current directory.
  > If that fails, then it should attempt to execute the command using the normal search
  > path that would be used by the shell.  Refer to the documentation on `execvp()`
  > for more information how to do this.

Once having set up the pipeline, the cook will use `wait()` or `waitpid()`
to await the completion of the processes in the pipeline and to determine whether
or not the pipeline succeeded.  The pipeline succeeds when every process exits normally
with an exit status of zero.  If a process in the pipeline exits abnormally
(*i.e.* by a signal) or it exits normally but with nonzero exit status, then the pipeline
fails.  If the pipeline succeeds, then the cook will go on to the next task for
that recipe.  If the pipeline fails, then the cook terminates with nonzero exit status
without performing any further tasks.  If the cook succeeds in performing all the tasks
in a recipe, then it terminates with exit status zero.

The main process in `cook` (*i.e.* the "chef") will generally be spending its time waiting
(in `sigsuspend()`) for a cook process to complete a recipe.  Consequently, it will need
previously to have arranged to receive a signal when a cook process terminates.
To achieve this, before beginning the main processing loop, the main process should
use the `sigaction()` system call to install a handler for `SIGCHLD` signals.
When this handler is invoked, indicating that a previously started cook process has
terminated, the following actions should be performed:

  * The exit status of the cook process that has terminated should be examined.
	If zero, the recipe on which the cook was working should be marked as "completed".
    If nonzero, the recipe on which the cook was working should be marked as "failed".

  * If the recipe completed successfully, then some recipes that depend on that recipe
	might now be ready to be processed as a result.  Any such recipes should be
    identified and added to the work queue, from which they will subsequently be considered
    for processing in the main processing loop.

Note that the main processing loop and the `SIGCHLD` handler are concurrent flows
that access the same shared data structure: the work queue.  Consequently, for reliable
operation it will be necessary for the main processing loop to block the execution of the
handler during times when the main loop is accessing the work queue.  The `sigprocmask()`
function should be used for this purpose.

  > **Important:**  Unless an error occurs during execution, your `cook` program must
  > run silently and not produce any output whatsoever, either on `stdout` or `stderr`,
  > when it is compiled and run in normal (*i.e.* non-debug) mode.
  > If an error occurs, you may (and should) print a one-line message to `stderr`
  > that announces the error.  You should **never** print any lines that begin either
  > with the words `START` or `END`.  This is because we intend to grade your program
  > by interpreting the output produced by commands that it runs, and extraneous
  > output produced by your program will potentially interfere with this.


## Provided Components

### The `cookbook.h` Header File

The file `cookbook.h` contains definitions of the data structures used to represent a
cookbook, as well as function prototypes for some parsing/unparsing functions I have
provided.

  > :scream: **Do not make any changes to `cookbook.h`.  It will be replaced
  > during grading, and if you change it, you will get a zero!**

```C
/*
 * A "cookbook" consists of a list of named "recipes".
 */
typedef struct cookbook {
    struct recipe *recipes;       // List of recipes in the cookbook.
    void *state;                  // Any additional state info you need to add.
} COOKBOOK;
```

The `cookbook` structure provides a generically typed `state` field, to which
you can assign a pointer to anything you like.  It is here because you might find that
you want to store additional information with a cookbook while `cook` is running,
but since you are not permitted to make any changes to these structure definitions
you would not be able to add another field to this structure.  If you want to
use this field, define (elsewhere, not in `cookbook.h`) the type of variable or structure
that you want to use and then at a suitable place in your code `malloc()` space
for this variable or structure and assign the pointer to the `state` field.
As accessing fields in your `malloc()`ed structure will be somewhat cumbersome due to
the required casts, consider defining some macros (again, not here, elsewhere)
that make it easier to do.

```C
/*
 * A "recipe" consists of a name, a list of "sub-recipes", and a sequence of "tasks".
 * In order to carry out the recipe, first the sub-recipes must be performed
 * (possibly in parallel), and then the specified tasks must be carried out in sequence.
 *
 * A "recipe_link" links from one recipe to another.  Recipe links are chained
 * together to form a list of sub-recipes on which a given recipe depends, and a list
 * of recipes that have a given recipe as a sub-recipe.
 */
typedef struct recipe_link {
    char *name;                       // Name of the linked recipe.
    struct recipe *recipe;            // The linked recipe.
    struct recipe_link *next;         // Next link in the dependency list.
} RECIPE_LINK;
```

Recipe links are present because recipes in a cookbook are not required to appear in
any particular order, and when the cookbook parser first encounters a reference to
a sub-recipe in the list of dependencies of another recipe, it might not yet have read
the definition for this subrecipe.  So a `recipe_link` structure is allocated and
just the name is filled in.  Later, once the entire cookbook has been parsed,
the cookbook is traversed and the `recipe` pointers in each `recipe_link` are filled in.
"Dangling" recipe references are also detected at that time.
These operations are taken care of for you by the cookbook parser I have provided.

One other reason why recipe links are present is to provide a place to store the `next`
pointer needed in order to chain dependencies into a list.

```C
typedef struct recipe {
    char *name;                       // Name of the recipe.
    RECIPE_LINK *this_depends_on;     // List of recipes on which this recipe depends.
    RECIPE_LINK *depend_on_this;      // List of recipes that depend on this recipe.
    struct task *tasks;               // Tasks to perform to complete the recipe.
    struct recipe *next;              // Next recipe in the cookbook.
    void *state;                      // Any additional state info you need to add.
} RECIPE;
```

The `this_depends_on` list in a recipe is the list of sub-recipes on which the recipe
depends.  These are the recipes whose names occur in the recipe header.
The `depend_on_this` list in a recipe contains the recipes that depend on this one.
This list can be used, for example, when a recipe completes, to easily identify
other recipes that might now be ready for processing.  You don't have to worry about
populating this list, it is done for you by the cookbook parser.

The `state` field in the `recipe` structure is provided for a similar reason as the
`state` field in `cookbook` structure.  You might find this field useful for keeping
track of whether a recipe is required, whether processing has started, whether the
recipe has failed, or whether it has completed.

```C
/*
 * A "task" consists of a sequence of "steps" to be performed by processes
 * in a pipeline, with the standard output of each command piped as the
 * standard input of the next command in the pipeline.
 * The tasks in a recipe are chained together into a list.
 * Each task may optionally include input or output redirections.
 * An input redirection is the name of a file (which must already exist) to
 * be used as the standard input to the first command in the pipeline.
 * An output redirection is the name of a file (which is created if it does
 * not exist and truncated if it does) to be used as the standard output of
 * the last command in the pipeline.
 */
typedef struct task {
    struct step *steps;         // Steps to perform to complete the task.
    char *input_file;           // Name of file for input redirection, or NULL.
    char *output_file;          // Name of file for output redirection, or NULL.
    struct task *next;          // Next task in the recipe.
} TASK;
```

```C
/*
 * A "step" consists of a NULL-terminated array of "words".  The first word
 * is interpreted as the name of a program to be run; the subsequent words are
 * its arguments.  The steps in a task are chained into a list.
 */
typedef struct step {
    char **words;               // NULL-terminated list of words.
    struct step *next;          // Next step in the task.
} STEP;
```

### The `cookbook_parser.o` Library

The `lib` directory contains an object file `cookbook_parser.o`, which contains my
implementation of a parser for cookbook files.
The two functions exported from this module are `parse_cookbook` and
`unparse_cookbook`.
The prototypes of these functions are given in `cookbook.h` and reproduced below.

```C
/*
 * Function for parsing a cookbook from an input stream.
 * This always returns a non-NULL cookbook, however, if errors are detected
 * during parsing it might be incomplete and should not be used.  In this
 * case the variable pointed at by errp is set to a nonzero value.
 * If parsing was successful, then this variable is set to zero.
 *
 * The caller is responsible for freeing the object returned by this function
 * and for managing the input stream and closing it (if appropriate).
 */
COOKBOOK *parse_cookbook(FILE *in, int *errp);
```

```C
/*
 * Function for outputting a cookbook to an output stream, in a format from
 * which it can be parsed again.
 *
 * The caller is reponsible for managing the output stream, including checking
 * it for errors and closing it (if appropriate).
 */
void unparse_cookbook(COOKBOOK *cbp, FILE *out);
```

### The `util` Directory and `generic_step` Program

The `util` directory contains the `generic_step` program that I wrote to create the
demonstration transcripts shown earlier.  You can use this program for your own
testing purposes.  When using `generic_step` you can use the same executable to
simulate commands with a variety of names, simply by creating multiple links to the
one executable.  For example, if you do:

```
ln -s generic_step AAA
ln -s generic_step BBB
ln -s generic_step CCC
```

You will be able to execute `AAA`, `BBB`, `CCC`, and they will each announce
themselves using the particular name by which they were invoked.

You can, of course, use real commands to test your program, as if you were using
`cook` as a replacement for `make`.  I strongly suggest that you do this,
as we will expect your program to work using real commands in addition to the
demonstration commands.

### The `rsrc` Directory

The `rsrc` directory contains a couple of demonstration cookbooks.
The file `eggs_benedict.ckb` contains the "Eggs Benedict" cookbook used as an example
in this document.  This cookbook uses only `generic_step` commands, to permit
the behavior of the `cook` program to be anayzed automatically.

The file `hello_world.ckb` contains a cookbook whose main recipe `hello_world`
invokes sub-recipes to create some C source files, compile and link them,
and then execute the result, which should print `Hello world!` on the terminal.
This file demonstrates how `cook` can perform the core functions of `make`,
using real commands that actually accomplish something.

You are strongly encouraged also to create your own test cookbooks to exercise
various aspects of your program's behavior, especially those that involve concurrency
and errors.  We will be grading your program by running it on a variety of test
cookbooks, using "commands" that produce output like that given in the example
transcripts in this document.  These transcripts will be analyzed to determine if
your program scheduled recipe steps in accordance with the dependence and sequencing
constraintsand whether it carried out each step as soon as possible,
subject to the `max_cooks` constraint.

### The `demo/cook` Demonstration Executable

To help you understand what you are to do, I have provided a demonstration
version of the program in binary form.  This can be found in the `demo`
directory.  For example, to run the "eggs benedict" example discussed above,
you can do the following:

```
$ demo/cook -c 10 -f rsrc/eggs_benedict.ckb
```

There is also a version of the demonstration program that has been compiled for
debugging and will produce an extensive debugging trace.
This version is called `demo/cook_db`.


### Test script

There is a test script in `tests/test_cook.py`.  It runs your `cook` program
on a cookbook and recipe that you specify and it analyzes the transcript output
generated by the steps that were performed, in order to determine whether the
main recipe completed successfully, the dependencies between the various
tasks and recipes were satisfied, and that tasks were started promptly whenever
there were sufficiently many cooks to handle them.  For example:

```
$ python3 tests/test_cook.py -f rsrc/eggs_benedict.ckb -c 10
```

will run `cook` on the "eggs benedict" recipe using 10 cooks and check the
results.  The `test_cook.py` script will only work for recipes that only use
the `generic_step` program to perform their steps.  This is because it uses
the `START` and `STOP` events produced by this program to keep track of the
progress of the recipes.

## Criterion Tests

The file `tests/base_tests.c` contains two tests, written using Criterion,
to test the functionality of your program.  These are both "black-box" tests,
rather than unit tests, in the sense that they run your program using `system()`
and analyze the output, rather than invoking functions within your code
directly.

The test `cook_basic_test` runs your program on the "eggs benedict" cookbook
and uses the `test_cook.py` script to check the results.

The tests `hello_world_test` runs your program on the "hello world" cookbook
and checks that the output produced is correct.

## Warning!

The program described here will exhibit complex concurrent behavior.
If you try to build the whole thing and debug it all at once you will have a very
difficult time getting it to work.  In particular, once you start using the main
processing loop to create "cook" processes and to handle the termination of
these processes by catching `SIGCHLD` signals, if there is any other part of
the program that is broken it might make it extremely difficult to identify
what is broken and to fix it.  What you should do is to make sure you test
individual pieces of your program as much as possible before trying to put it
all together.   For example, it might well be a good idea to first implement
a sequential version of the program, without separate "cook" processes,
just to make sure the mechanisms of the work queue, the main processing loop,
and how completing recipes trigger other recipes that depend on them,
all work correctly.  Once you have some confidence in this sequential version
of the program, you can make the modifications necessary to introduce concurrent
processing.

## Hand-in instructions
As usual, make sure your homework compiles before submitting.
Test it carefully to be sure that doesn't crash or exhibit "flaky" behavior
due to race conditions.
We have not supplied any test cases this time, but you are encouraged to
create your own unit tests for as you implement the pieces of this program.

Submit your work using `git submit` as usual.
This homework's tag is: `hw4`.
