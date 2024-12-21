# Homework 1 - CSE 320
#### Professor Eugene Stark

**Read the entire doc before you start**

## Introduction

In this assignment, you will write a command line utility to perform data
compression via Huffman coding.
The goal of this homework is to familiarize yourself with C programming,
with a focus on input/output, strings in C, bitwise manipulations,
and the use of pointers.

For all assignments in this course, you **MUST NOT** put any of the functions
that you write into the `main.c` file.  The file `main.c` **MUST ONLY** contain
`#include`s, local `#define`s and the `main` function (you may of course modify
the `main` function body).  The reason for this restriction has to do with our
use of the Criterion library to unit test your code.
Beyond this, you may have as many or as few additional `.c` files in the `src`
directory as you wish.  Also, you may declare as many or as few headers as you wish.
Note, however, that header and `.c` files distributed with the assignment base code
often contain a comment at the beginning which states that they are not to be
modified.  **PLEASE** take note of these comments and do not modify any such files,
as they will be replaced by the original versions during grading.
In this document, we use `huff.c` as our example file containing helper functions.

> :scream: Array indexing (**'A[]'**) is not allowed in this assignment.
> You **MUST USE** pointer arithmetic instead.
> **No** array brackets (**'[', ']'**) are allowed.
> This means you cannot declare your own arrays.
> All necessary arrays are declared in the `global.h` header file.
> You **MUST USE** these arrays. **DO NOT** create your own arrays.
> We **WILL** check for this.

> :nerd: Reference for pointers: [https://beej.us/guide/bgc/html/#pointers](https://beej.us/guide/bgc/html/#pointers).

# Getting Started

Fetch base code for `hw1` as described in `hw0`. You can find it at this link:
[https://gitlab02.cs.stonybrook.edu/cse320/hw1](https://gitlab02.cs.stonybrook.edu/cse320/hw1).
**IMPORTANT: 'FETCH AND MERGE', DO NOT 'CLONE'.**

Both repos will probably have a file named `.gitlab-ci.yml` with different contents.
Simply merging these files will cause a merge conflict. To avoid this, we will
merge the repos using a flag so that the `.gitlab-ci.yml` found in the `hw1`
repo will be the file that is preserved.
To merge, use this command:

```
git merge -m "Merging HW1_CODE" HW1_CODE/master --strategy-option=theirs
```

> :scream: Based on past experience, many students will either ignore the above command or forget
> to use it.  The result will be a **merge conflict**, which will be reported by git.
> Once a merge conflict has been reported, it is essential to correct it before committing
> (or to abort the merge without committing -- use `git merge --abort` and go back and try again),
> because git will have inserted markers into the files involved indicating the locations of the
> conflicts, and if you ignore this and commit anyway, you will end up with corrupted files.
> You should consider it important to read up at an early stage on merge conflicts with git and
> how to resolve them properly.

Here is the structure of the base code:

<pre>
.
├── .gitignore
├── .gitlab-ci.yml
└── hw1
    ├── hw1.sublime-project
    ├── include
    │   ├── debug.h
    │   ├── global.h
    │   └── huff.h
    ├── Makefile
    ├── rsrc
    │   ├── gettysburg.out
    │   └── gettysburg.txt
    ├── src
    │   ├── huff.c
    │   └── main.c
    └── tests
        └── basecode_tests.c
</pre>

- The `.gitignore` file is a file that tells `git` to ignore files with names
matching specified patterns, so that they don't accidentally end up getting
committed to the repository.

- The `.gitlab-ci.yml` file is a file that specifies "continuous integration" testing
to be performed by the GitLab server each time you push a commit.  Usually it will
be configured to check that your code builds and runs, and that any provided unit tests
are passed.  You are free to change this file if you like.

> :scream:  The CI testing is for your own information; it does not directly have
> anything to do with assignment grading or whether your commit has been properly
> pushed to the server.  If some part of the testing fails, you will see the somewhat
> misleading message "commit failed" on the GitLab web interface.
> This does **not** mean that "your attempt to commit has failed" or that "your commit
> didn't get pushed to the server"; the very fact that the testing was triggered at
> all means that you successfully pushed a commit.  Rather, it means that "the CI tests
> performed on a commit that you pushed did not succeed".  The purpose of the tests are
> to alert you to possible problems with your code; if you see that testing has failed
> it is worth investigating why that has happened.  However, the tests can sometimes
> fail for reasons that are not your fault; for example, the entire CI "runner" system
> may fail if someone submits code that fills up the system disk.  You should definitely
> try to understand why the tests have failed if they do, but it is not necessary to be
> overly obsessive about them.

- The `hw1.sublime-project` file is a "project file" for use by the Sublime Text editor.
It is included to try to help Sublime understand the organization of the project so that
it can properly identify errors as you edit your code.

- The `Makefile` is a configuration file for the `make` build utility, which is what
you should use to compile your code.  In brief, `make` or `make all` will compile
anything that needs to be, `make debug` does the same except that it compiles the code
with options suitable for debugging, and `make clean` removes files that resulted from
a previous compilation.  These "targets" can be combined; for example, you would use
`make clean debug` to ensure a complete clean and rebuild of everything for debugging.

- The `include` directory contains C header files (with extension `.h`) that are used
by the code.  Note that these files often contain `DO NOT MODIFY` instructions at the beginning.
You should observe these notices carefully where they appear.

- The `src` directory contains C source files (with extension `.c`).

- The `tests` directory contains C source code (and sometimes headers and other files)
that are used by the Criterion tests.

- The `rsrc` directory contains some samples of data files that you can use for
testing purposes.

## A Note about Program Output

What a program does and does not print is VERY important.
In the UNIX world stringing together programs with piping and scripting is
commonplace. Although combining programs in this way is extremely powerful, it
means that each program must not print extraneous output. For example, you would
expect `ls` to output a list of files in a directory and nothing else.
Similarly, your program must follow the specifications for normal operation.
One part of our grading of this assignment will be to check whether your program
produces EXACTLY the specified output.  If your program produces output that deviates
from the specifications, even in a minor way, or if it produces extraneous output
that was not part of the specifications, it will adversely impact your grade
in a significant way, so pay close attention.

> :scream: Use the debug macro `debug` (described in the 320 reference document in the
> Piazza resources section) for any other program output or messages you many need
> while coding (e.g. debugging output).

# Part 1: Program Operation and Argument Validation

In this part, you will write a function to validate the arguments passed to your
program via the command line. Your program will treat arguments as follows:

- If no flags are provided, you will display the usage and exit with an
`EXIT_FAILURE` status.

- If the `-h` flag is provided, you will display the usage for the program and
  exit with an `EXIT_SUCCESS` status.

- If the `-c` flag is provided, you will perform data compression; reading
  uncompressed data from `stdin` and writing compressed data to `stdout`,
  exiting with `EXIT_SUCCESS` on success and `EXIT_FAILURE` on any error.

- If the `-d` flag is provided, you will perform decompression; reading
  compressed data from `stdin` and writing uncompressed data to `stdout`,
  exiting with `EXIT_SUCCESS` on success and `EXIT_FAILURE` on any error.

> :nerd: `EXIT_SUCCESS` and `EXIT_FAILURE` are macros defined in `<stdlib.h>` which
> represent success and failure return codes respectively.

> :nerd: `stdin`, `stdout`, and `stderr` are special I/O "streams", defined
> in `<stdio.h>`, which are automatically opened at the start of execution
> for all programs, do not need to be reopened, and (almost always) should not
> be closed.  In lieu of closing `stdout` or `stderr`, in certain circumstances
> (such as at the end of execution) it might be necessary to `fflush()` these
> output streams.

> :scream: Any libraries that help you parse strings (`string.h`, `ctype.h`, etc.)
> are **prohibited** for this assignment.  The use of `atoi`, `scanf`, `fscanf`, `sscanf`,
> and similar functions is likewise prohibited.  *This is intentional and
> will help you practice parsing strings and manipulating pointers.*

The usage scenarios for this program are described by the following message,
which is printed by the program when it is invoked without any arguments:

Some of these operations will also need other command line arguments which are
described in each part of the assignment.  The two usage scenarios for this program are:

<pre>
Usage: bin/huff [-h] [-c|-d] [-b BLOCKSIZE]
    -h       Help: displays this help menu
    -c       Compress: read raw data, output compressed data
    -d       Decompress: read compressed data, output raw data
    -b       For compression, specify blocksize in bytes (range [1024, 65536])
</pre>

The square brackets in the above message indicate that the enclosed argument is optional.
The '|' indicates a choice between alternative arguments.
A valid invocation of the program implies that the following hold about
the command-line arguments:

If -h is specified, then it must be the first option on the command line, and any
other options are ignored.

The `[-c|-d]` means that one or the other of `-c` or `-d` may be specified.
The `[-b BLOCKSIZE]` means that `-b` may be optionally specified, in which
case it is immediately followed by a parameter `BLOCKSIZE`.

A valid invocation of the program implies that the following hold about
the command-line arguments:

- The arguments (`-h` `-d` `-c`) are positional arguments which, if present,
must appear before any option arguments (`-b`). The option arguments
(in this case `-b` is the only such argument) may come in any order after
the positional ones.

- If the `-h` flag is provided, it is the first positional argument after
the program name and any other arguments that follow are ignored.

	> If the `-h` flag is *not* specified, then exactly one of `-d`, or `-c`
	> must be specified.

- The `-b` option may only be given together with the `-c` option.

- If an option requires a parameter, the corresponding parameter must be provided
(e.g. `-b` must always be followed by a BLOCKSIZE specification).

    - If `-b` is given, the BLOCKSIZE argument will be given as a decimal integer in
    the range [1024, 65536].

For example, the following are a subset of the possible valid argument
combinations:

- `$ bin/huff -h ...`
- `$ bin/huff -c -b 1024`
- `$ bin/huff -d`

> :scream: The `...` means that all arguments, if any, are to be ignored; e.g.
> the usage `bin/huff -h -x -y BLAHBLAHBLAH -z` is equivalent to `bin/huff -h`.

Some examples of invalid combinations would be:

- `$ bin/huff -c -d -b 1024`
- `$ bin/huff -b 1024 -c`
- `$ bin/huff -d -b 1024`
- `$ bin/huff -c -b 1k`

> :scream: You may use only "raw" `argc` and `argv` for argument parsing and
> validation. Using any libraries that parse command line arguments (e.g.
> `getopt`) is prohibited.

> :scream: Any libraries that help you parse strings are prohibited as well
> (`string.h`, `ctype.h`, etc). *This is intentional and will help you
> practice parsing strings and manipulate pointers.*

> :scream: You **MAY NOT** use dynamic memory allocation in this assignment
> (i.e. `malloc`, `realloc`, `calloc`, `mmap`, etc.).

> :nerd: Reference for command line arguments: [https://beej.us/guide/bgc/html/#command-line-arguments](https://beej.us/guide/bgc/html/#command-line-arguments).

**NOTE:** The `make` command compiles the `huff` executable into the `bin` folder.
Assume all commands in this doc are run from from the `hw1` directory of your
repo.

### **Required** Validate Arguments Function

In `global.h`, you will find the following function prototype (function
declaration) already declared for you. You **MUST** implement this function
as part of the assignment.

```c
int validargs(int argc, char **argv);
```

The file `validargs.c` contains the following specification of the required behavior
of this function:

```c
/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
```

> :scream: This function must be implemented as specified as it will be tested
> and graded independently. **It should always return -- the USAGE macro should
> never be called from validargs.**

The `validargs` function should return -1 if there is any form of failure.
This includes, but is not limited to:

- Invalid number of arguments (too few or too many)

- Invalid ordering of arguments

- A missing parameter to an option that requires one (e.g. `-b` with no
  `BLOCKSIZE` specification).

- Invalid `BLOCKSIZE` (if one is specified).  A `BLOCKSIZE` is invalid if it
contains characters other than the digits ('0'-'9'), or if it denotes a
value not in the range [1024, 65536].

The `global_options` variable of type `int` is used to record the mode
of operation (i.e. help/compress/decompress) of the program, as well as
the blocksize.  This is done as follows:

- If the `-h` flag is specified, the least significant bit (bit 0) is 1.

- The second-least-significant bit (bit 1) is 1 if `-c` is passed
(i.e. the user wants compression mode).

- The third-least-significant bit (bit 2) is 1 if `-d` is passed
(i.e. the user wants decompression mode).

- If the `-b` option was specified, then the given blocksize, minus one,
is recorded in the 16 most-significant bits of `global_options`,
otherwise the 16 most-significant bits of `global_options` are 0xffff
(representing the default block size of `65536`).

If `validargs` returns -1 indicating failure, your program must call
`USAGE(program_name, return_code)` and return `EXIT_FAILURE`.
**Once again, `validargs` must always return, and therefore it must not
call the `USAGE(program_name, return_code)` macro itself.
That should be done in `main`.**

If `validargs` sets the least-significant bit of `global_options` to 1
(i.e. the `-h` flag was passed), your program must call `USAGE(program_name, return_code)`
and return `EXIT_SUCCESS`.

> :nerd: The `USAGE(program_name, return_code)` macro is already defined for you
> in `global.h`.

If validargs returns 0, then your program must read data from `stdin`,
either compressing it or decompressing it as specified by the values of
`global_options` and `block_size`, and writing the result to `stdout`.
Upon successful completion, your program should exit with exit status `EXIT_SUCCESS`;
otherwise, in case of an error it should exit with exit status `EXIT_FAILURE`.

If `-b` is provided, you must check to confirm that the specified block size
is valid.

Unless the program has been compiled for debugging (using `make debug`),
in a successful run that exits with `EXIT_SUCCESS` no unspecified output may be produced
by the program.  In an unsuccessful run in which the program exits with `EXIT_FAILURE`
the program should output to `stderr` an error message or messages that indicate
the reason for the failure.

> :nerd: Remember `EXIT_SUCCESS` and `EXIT_FAILURE` are defined in `<stdlib.h>`.
> Also note, `EXIT_SUCCESS` is 0 and `EXIT_FAILURE` is 1.

> :nerd: We suggest that you create functions for each of the operations defined
> in this document. Writing modular code will help you isolate and fix
> problems.

### Sample validargs Execution

The following are examples of `global_options` settings for given inputs.
Each input is a bash command that can be used to run the program.

- Input: `bin/huff -h`.  Setting: 0x1 (`help` bit is set, other bits clear).

- Input: `bin/huff -c -b 1024`.  Setting: 0x3ff0002 (`compress` bit is set;
`blocksize` bits are 0x3ff, representing block size `1024`).

- Input: `bin/huff -d`.  Setting: 0xffff0004 (`decompress` bit is set;
`blocksize` bits are 0xffff, representing block size `65536`).

- Input: `bin/huff -b 1024 -c`.  Setting: 0x0. This is an error
case because the specified argument ordering is invalid (`-b` is before `-c`).
In this case `validargs` returns 0, leaving `global_options` unset.

# Part 2: Data Compression using Huffman Coding

This section gives very basic information on Huffman coding and its use in data
compression.  Only as much information as is needed to implement the algorithms
is included.  If you would like to learn more about the subject, you can refer
to the following references:

- Wikipedia article [Huffman Coding](https://en.wikipedia.org/wiki/Huffman_coding) on Huffman coding.
- Wikipedia article [DEFLATE](https://en.wikipedia.org/wiki/DEFLATE) on the `DEFLATE` algorithm,
  which is used in popular compression programs such as `gzip`, and which uses Huffman coding as a
  part.

First, some very basic coding theory terminology.  Data to be compressed is expressed as a
sequence of *symbols* drawn from some finite alphabet.  For us, a symbol is a single byte,
which has an unsigned integer value in the range [0, 255], and we will be interested in
compressing data expressed as a sequence of bytes.  The output of our compression algorithm
will logically be a *bit stream*, which is a sequence of 0/1 values, though this bit stream
will be re-blocked into 8-bit bytes because that is the granularity at which our computers and
software are designed to work.  Compression is performed by replacing each input byte by a
corresponding sequence of bits according to a *code book*, or mapping, that we will generate
for the purpose.  The code book is set up so that, rather than using a sequence of bits of a
fixed length (*e.g.* 8) to represent each input symbol, we will use short sequences of bits
to represent input symbols that occur very frequently and longer sequences of bits to represent
input symbols that occur less frequently.  Using such a variable-length encoding can often make
it possible to represent the same data more compactly.

If we are given a code book that maps each byte value to a corresponding sequence of bits,
then we can encode a sequence of bytes simply by looking up each successive byte in the code book
and then outputting the corresponding code (sequence of bits).  However, there is a slight problem:
in order to decode the bit sequence and recover the original data, we have to be able to determine
where the code for one byte ends and another begins.  This is where the concept of a
*prefix-free code* comes in.  A prefix-free code has the property that the codes assigned to
distinct symbols are never prefixes of each other.  This property allows us to read data encoded
as a sequence of bits and to separate it unambiguously into the codes for individual input symbols.
A prefix-free code is conveniently represented using a *Huffman tree*, which is an ordered binary
tree whose leaves are labeled by distinct symbols, and where the code for each symbol
is given by the path from the root to the corresponding leaf, with bit 0 corresponding to
going from a node to its left child and with bit 1 corresponding to going from a node to its
right child.  If we have such a tree, then we can decode an encoded bit stream by starting at
the root, reading successive bits and following the path they indicate, until we come to
a leaf, at which point we output the symbol at the leaf and go back to the root to repeat
the process.

Our data compression algorithm will work as follows:  Input data to be compressed will be read
in *blocks* that consist of a fixed number of bytes (except at the end of the input, where we
might have only a partial block).  As we read each block, we will tally the number of times that
each byte value occurs in the input, resulting in a histogram.  We will then use this histogram
to build a Huffman tree corresponding to an optimal code.  The input data will be encoded as a
bit sequence, using the codebook represented by the Huffman tree, and then output,
but before doing that we will first output a description of the Huffman tree, so that
it will be available for use in decoding.  Therefore, the result of compressing a block will
consist of a description of a Huffman tree, followed by a bit sequence that was produced by
encoding the input data using the code defined by the tree.

Our decompression algorithm will also work in a block-wise fashion.  To decode a block,
first the description of a Huffman tree will be read and the tree will be reconstructed from
the description.  Then the encoded block data will be read as a sequence of bits and by following
paths in the tree as described above, we will be able to recover the original data.
There is one slight catch -- during decoding we will need to be able to detect the end of
each block so that we know when to stop decoding data and to start reading the Huffman tree
for the next block.  In order to handle this, we will arrange for our codes to include a special
"block end" symbol in addition to the 256 byte values.  When we are decoding data, if we
identify the code for the "block end" symbol, we finish with the current block and go on
to the next one.

To clarify things, suppose we wish to encode the input data `abcbcc`.  Represented as a sequence
of bytes, this is `97` `98` `99` `98` `99` `99` (the ASCII codes for `a`, `b`, and `c` are
`97`, `98`, and `99`, respectively).  If the block size is at least 6, then this data will be
read in as a single block, and we will construct the histogram `{a: 1, b: 2, c: 3}` that gives
the frequency of occurrence of each byte.  Let us assign the "end block" symbol a frequency of `0`.
Next, we construct a Huffman tree from this frequency data (more later on how to do that).
This results in a codebook (not uniquely determined) that maps `a` to `101`, `b` to `11`, `c` to `0`,
and the "end block" symbol to `100`.  The corresponding Huffman tree has seven nodes, four of which
are leaves:

<pre>
             /\
           0/  \1
           /    \
          c     /\
              0/  \1
              /    \
             /\     b
           0/  \1
           /    \
         END     a
</pre>

We now use the codebook to encode the input data, resulting in the bit sequence
`101 11 0 11 0 0 100` (the spaces have been inserted for clarity; they are not
are not part of the bit sequence), or the two hexadecimal bytes: `BB 20`
(after adding three `0` bits of padding at the end to bring the total number of bits
to a multiple of 8).
Note that whereas the original data required 48 bits, the encoded bit sequence
is now only 16 bits long.  However, in this short example we won't actually end up
achieving any compression, because we need additional bits to describe the
Huffman tree, and the total ends up being longer than the original input data.
The particular scheme we will use to describe the tree results in the following
56-bit description (in hexadecimal, explanation later on):

  `00 07 16 63 FF 00 61 62`

With more input and a big enough block size, we would be able to get enough
reduction in the size of the input data to compensate for the overhead of having
to include the Huffman tree in the output.

To decode the bit sequence, we first have to read the description of the Huffman
tree and reconstruct it.  Then we read the encoded data and follow the tree:
`101` from the root takes us to the leaf labeled `c`, `11` from the root takes
us to the leaf labeled `b`, and so on until `100` from the root takes us to
the leaf labeled `END`, which indicates the end of the block.  The three `0`
bits of padding are skipped.

## Constructing a Huffman Tree

Constructing a Huffman tree from a histogram is easy.  First, create leaf nodes
for each of the symbols, storing the frequency count for each symbol in a
"weight" field of the node.  Then, repeat the following until just one node
remains: Select two nodes of minimum weight, remove them, and replace them
by a new node having the selected nodes as its children, and having the sum of
the weights of the selected nodes as its weight.  When just one node remains,
the tree is complete, and the remaining node is the root.  For efficient
implementation, the appropriate data structure for maintaining the set of nodes
is a priority queue, using the weight as the priority, but the set of nodes
can of course also simply be maintained as a list, and minimum-weight nodes
selected via linear search.

## Describing a Huffman Tree

A Huffman tree could be described by simply outputting the contents of
each of the nodes, using node numbers to replace the left and right child pointer
fields.  However, such a description is overly verbose, and since we are trying
to do data compression, we would rather use a more succinct scheme for describing
a tree.  In our application, the description of a Huffman tree will consist
of the following:

  * The number of nodes n, given as a two-byte sequence in big-endian order.

  * A sequence of n bits, derived from a postorder traversal of the tree,
    in which each 0 bit indicates a leaf and each 1 bit denotes an internal node.
    The sequence is padded at the end with up to seven 0 bits, if necessary,
    to reach a length that is a multiple of 8 bits, so that this portion of the
    description occupies an integral number of bytes.

> :nerd: Recall that a postorder traversal of an ordered binary tree is performed
> by the recursive algorithm ``visit the left subtree, visit the right subtree,
> visit the root''.

  * A sequence of bytes that gives the values of the symbols at the (n+1)/2
	leaves of the tree.  Each single byte gives the value at one leaf of the tree,
    with the following exceptions: (1) the two-byte sequence 255, 0 specifies
    a special "end block" marker; and (2) any other two-byte sequence whose first
    byte is 255 represents the value 255.

In the example given above, the first two bytes of the description (`00 07`)
give the number of nodes: 7, from which the number of leaves: (7+1)/2 = 4
can also be calculated.  The next byte, `16` in hex or `00010110` in binary,
represents the 7-bit sequence `0001011` with an additional 0 added as padding
to reach 8 bits.  A postorder traversal of the tree visits the nodes
in the order `c`, `END`, `a`, (internal), `b`, (internal), (root), which
with `0` for a leaf and `1` for a non-leaf, yields the indicated sequence.
The remaining 5 bytes: `63 FF 00 61 62` specify the symbol values at the
leaves from left to right: 'c', 'END', 'a', 'b'.

A tree described in this way can be readily reconstructed from the description.
Take the first two bytes and combine them to determine the number of nodes n,
as well as the number of leaves (n+1)/2.
Next, scan through the bit sequence derived from the postorder traversal and
use it to build the tree.  This requires the use of a stack of nodes, which
is initially empty.  When `0` is scanned, create a new leaf node and push it
on the stack.  When `1` is scanned, pop two nodes, call the first node popped "R"
and the second "L", create a new node "P" having L as its left child and R
as its right child, and push P onto the stack.
Once n bits have been scanned in this way, there will be a single node
remaining on the stack, which is the root of the tree.
Finally, read through the sequence of bytes that give the symbol values,
and save the values at the leaf nodes, working from left to right in the tree.
To accomplish this, it will have been useful, during the building of the tree,
to have initialized an auxiliary array that maps each index in [0, (n+1)/2)
to the corresponding leaf node.

# Part 3: The Header File `huff.h`

The header file `huff.h` contains the definition of a type `NODE` to
represents a node in a Huffman tree, as well as the declarations of several
variables and arrays.  You **must** use the arrays declared here to store your
data structures -- you are **not** permitted to declare any arrays elsewhere,
nor to use array subscripting `[` `]`, and you are **not** permitted to use
any dynamic storage allocation, such as `malloc()`.

The `NODE` type is defined as follows:

```c
typedef struct node {
    struct node *left;      // Pointer to left child
    struct node *right;     // Pointer to right child
    struct node *parent;    // Pointer to parent
    int weight;             // Weight (priority) of node
    short symbol;           // Symbol at leaf node
} NODE;
```

Associated with this is the definition of a `nodes` array:

```c
NODE nodes[2*MAX_SYMBOLS - 1];
```

As a tree with `n` symbols has exactly `2*n-1` nodes, you will not need any
more nodes than are provided by this array.
There is also a variable into which to store the number of nodes of the
`nodes` array that are actually used:

```c
int num_nodes;
```

> :scream: You **must** make sure that once your Huffman tree is built,
> it occupies a contiguous sequence of nodes in the `nodes` array,
> with the root node at index `0`, and that `num_nodes` has been set to
> correctly reflect the number of nodes in your tree.  When we test your
> program, we need to assume that you have done this in order that we can
> check your Huffman tree for correctness.

An auxiliary array has been provided that you can use to map symbol values
to the corresponding leaf nodes while you are constructing (or reconstructing)
a Huffman tree.  We don't care about the content of this array, but it is
defined in this header file because we expect you will need it and you are
not permitted to allocate any arrays elsewhere.

```c
NODE *node_for_symbol[MAX_SYMBOLS];
```

Finally, an array is provided for you to use to store the content of the
current block, between the time is has been read in and the time the compressed
data is output:

```c
unsigned char current_block[MAX_BLOCK_SIZE];
```

# Part 4: **Required** Functions

In order to provide some additional structure for you, as well as to make it
possible for us to perform additional unit tests on your program,
you are required to implement the following functions as part of your program.
The prototypes for these functions are given in `global.h`.
Once again, you **MUST** implement these functions as part of the assignment,
as we will be testing them separately.

```c
/**
 * @brief Reads raw data from standard input, writes compressed data to
 * standard output.
 * @details This function reads raw binary data bytes from the standard input in
 * blocks of up to a specified maximum number of bytes or until EOF is reached,
 * it applies a data compression algorithm to each block, and it outputs the
 * compressed blocks to standard output.  The block size parameter is obtained
 * from the global_options variable.
 *
 * @return 0 if compression completes without error, -1 if an error occurs.
 */
int compress();
```

```c
/**
 * @brief Reads compressed data from standard input, writes uncompressed
 * data to standard output.
 * @details This function reads blocks of compressed data from the standard
 * input until EOF is reached, it decompresses each block, and it outputs
 * the uncompressed data to the standard output.  The input data blocks
 * are assumed to be in the format produced by compress().
 *
 * @return 0 if decompression completes without error, -1 if an error occurs.
 */
int decompress();
```

```c
/**
 * @brief Reads one block of data from standard input and emits corresponding
 * compressed data to standard output.
 * @details This function reads raw binary data bytes from the standard input
 * until the specified block size has been read or until EOF is reached.
 * It then applies a data compression algorithm to the block and outputs the
 * compressed block to the standard output.  The block size parameter is
 * obtained from the global_options variable.
 *
 * @return 0 if compression completes without error, -1 if an error occurs.
 */
int compress_block();
```

```c
/**
 * @brief Reads one block of compressed data from standard input and writes
 * the corresponding uncompressed data to standard output.
 * @details This function reads one block of compressed data from the standard
 * input, it decompresses the block, and it outputs the uncompressed data to
 * the standard output.  The input data blocks are assumed to be in the format
 * produced by compress().  If EOF is encountered before a complete block has
 * been read, it is an error.
 *
 * @return 0 if decompression completes without error, -1 if an error occurs.
 */
int decompress_block();
```

```c
/**
 * @brief Emits a description of the Huffman tree used to compress the current block.
 * @details This function emits, to the standard output, a description of the
 * Huffman tree used to compress the current block.  Refer to the assignment handout
 * for a detailed specification of the format of this description.
 */
void emit_huffman_tree();
```

```c
/**
 * @brief Reads a description of a Huffman tree and reconstructs the tree from
 * the description.
 * @details  This function reads, from the standard input, the description of a
 * Huffman tree in the format produced by emit_huffman_tree(), and it reconstructs
 * the tree from the description.  Refer to the assignment handout for a specification
 * of the format for this description, and a discussion of how the tree can be
 * reconstructed from it.
 *
 * @return 0 if the tree is read and reconstructed without error, otherwise -1
 * if an error occurs.
 */
int read_huffman_tree();
```

# Part 5: Implementation Notes

The program will read data byte-by-byte from the standard input and write data
byte-by-byte to the standard output.  You should use `fgetc()` to read an input
byte and `fputc()` to write an output byte.  Once all the output has been
produced, you should use `fflush()` to make sure that it has all been emitted
to the output file.  You might find it useful to use `feof()` and `ferror()`
to check for an EOF or error condition, respectively, on the input or output
stream.  You should not need any I/O functions other than these.

The functions you are required to implement can be coded without having to
allocate any dynamic storage or any arrays other than the ones you have been given.

While you are reading in a block, you can use the initial portion of the `nodes`
array to store (what will eventually be) the leaf nodes of your Huffman tree.
The `symbol` field should be used to store the symbol value associated with a
node and the `weight` field should be used to store the number of times that
symbol value occurs in the block.

Once a block has been read and the symbol frequencies tallied, you will build
the Huffman tree as described previously.  This can be done "in place" in the
nodes array, in such a way that no other node storage will be required and
the finished tree will occupy a contiguous set of nodes with the root node at
index `0` and the last node at index `n-1`, where `n` is the number of nodes.
To do this, while the tree is being constructed, maintain the nodes that have
not yet been assigned a parent in the tree in the initial portion of the
nodes array (initially, this will contain the `(n+1)/2`leaf nodes).  Each time you
select two minimum-weight nodes, move their contents to the "high end" of
the final region, and use the space originally occupied by one of the nodes
to hold the parent node.  For example, when you select the first two minimum-weight
nodes, you will move their contents to indices `n-1` and `n-2`, and you will put
the parent node that points to them at index `(n+1)/2-2`, maintaining a contiguous
set of nodes in the "low end" of the `nodes` array.  Note that, depending on how
you do things, it might be necessary to shift things over to fill up
the space vacated by the minimum-weight nodes that were moved and maintain
contiguity.  A slick way to do this while making it possible to efficiently
identify minimum weight nodes is to use the "low end" of the `nodes` as a binary
heap, in which case the algorithms for removing the minimum weight nodes will
naturally maintain the nodes in a contiguous sequence, but you don't have to do
it that way if you don't want to.
As the tree is constructed, the number of nodes in the "low end" will decrease
and nodes that have been assigned parents will find a "permanent home" in the
"high end".  Eventually all the nodes but one will be in the "high end"
and the node remaining in the "low end" will be the root.

Once the tree has been built, you can perform a single traversal to identify
the leaf nodes and install pointers to them in the `node_for_symbol` array.
You will use this array during compression in order to find the leaf node
in the tree that corresponds to each input symbol.
The parent pointers in the `NODE` structure are needed in order to read out
the codes for input symbols during data compression.  You can set the parent
pointers as you build the tree (this is slightly tricky to get right) or you
can traverse the tree once it has been built to set these pointers.
To obtain the code for an input symbol `s`, use `s` as an index into the
`node_for_symbol` array to obtain a pointer to the corresponding leaf node
in the tree.  Following the parent pointers will lead you back to the root
of the tree, and the directions that you "come from" at each step in this
path will correspond to the bits in the code for that symbol.
There is only one slight catch: if you output these bits as you trace back
the path from the leaf to the root they will be in the opposite order from
what is needed during decompression, so they have to be reversed.
To do this without any additional storage, as you trace back from a
leaf to the root use the `weight` fields along the path (which are no longer
needed now that the tree has been built) to store the "come from" directions.
Then, start at the root and follow the "come from" directions to trace
the path from root to leaf, outputting the code bits in the proper order
as you do so.

When reading the description of a tree during decompression, use the low
end of the `nodes` array as the stack required to build the tree, and then
as nodes are assigned parents, move them to a permanent home in the "high end"
of the `nodes` array.  For each `0` bit in the input sequence, use the first
node to the right of the stack pointer as the new input node and "push" this
node by incrementing the stack pointer.  For each `1` bit in the input sequence,
move the contents of the top two nodes on the stack to the first two unused
nodes in the "high end" of the array and use the lower-numbered one of the
two nodes vacated to store the new parent node.  The net change to the
stack pointer will be to decrement it by one (two pops, one push).
Parent pointers can be filled in as the tree is constructed (carefully!)
or during a single traversal performed once the tree is complete.

# Part 5: Running the Completed Program

In any of its operating modes, the `huff` program reads from `stdin` and writes
to `stdout`.  As the input and output of the program is binary data, it will not
be useful to enter input directly from the terminal or display output directly to
the terminal.  Instead, the program can be run using *input and output redirection*
as in the following example:

```
$ bin/huff -c -b 1024 < infile > outfile
$ echo $?
0
```

This will cause the input to the program to be redirected from the file `infile`
and the output from the program to be redirected to the file `outfile`.

> :nerd: The `<` symbol tells the shell to perform input redirection:
> input is taken from the file `infile` (which must exist) instead of
> from the terminal.
> The `>` symbol tells the shell to perform "output redirection":
> the file `outfile` is created (or truncated if it already existed -- be careful!)
> and the output produced by the program is sent to that file instead
> of to the terminal.
> It is important to understand that redirection
> is handled by the shell and that the `bin/huff` program never sees any
> of the redirection arguments; in the above example it sees only `bin/huff -c -b 1024`
> and it just reads from `stdin` and writes to `stdout`.

> :nerd: `$?` is an environment variable in bash which holds the return code of
> the previous program run.  In the above, the `echo` command is used to display
> the value of this variable.

Besides redirecting output to a file, the output from a command can be **piped**
to another program, without the use of a disk file.
This could be done, for example, by the following command:

```
$ bin/huff -d < in | less
```

This sends the output to a program called `less`, which displays the first
screenful of the output and then gives you the ability to scan forward and
backward to see different parts of it.  Type `h` at the `less` prompt to get
help information on what you can do with it.  Type `q` at the prompt to exit `less`.
The `less` program is indispensible for viewing lengthy output produced by
a program.

Pipelines are a powerful tool for combining simple component programs into
combinations that perform complex transformations on data.

We just mention one other useful command that is often used with pipelines;
namely `cat`.
The `cat` command (short for "concatenate and print") is a command that reads
files specified as arguments, concatenates their contents, and prints the result
to `stdout`.
For example, an alternative way to send the contents of a file `in` as input
to `huff` is the following:

```
cat in | bin/huff -c
```

For debugging purposes, the contents of `outfile` can be viewed using the
`od` ("octal dump") command:

> <pre>
> $ od -t x1 outfile
> 0000000 00 07 16 63 ff 00 61 62 bb 20
> 0000012
> </pre>

This is the compressed output from the `abcbcc` example discussed above.
The successive bytes in the file have the hexadecimal values
`00 07 16 63 ff 00 61 62 bb 20`, as previously discussed.
The values in the first column indicate the offsets from the beginning of the file,
specified as 7-digit octal (base 8) numbers.

> :nerd: The `-t x1` flag instructs `od` to interpret the file as a sequence of
> individual bytes (that is the meaning of the "`1`" in "`x1`"), which are printed as
> hexadecimal values (that is the meaning of the "`x`" in "`x1`").  The `od` program has
> many options for setting the output format; another useful version is `od -bc`,
> which shows individual bytes of data as both ASCII characters and their octal codes.
> Refer to the "man" page for `od` for other possiblities.

If you use the above command with an `outfile` that is much longer, there would
be so much output that the first few lines would be lost off of the top of the screen.
To avoid this, you can *pipe* the output to `less`:

> <pre>
> $ od -t x1 outfile | less
> </pre>

This will display only the first screenful of the output and give you the
ability to scan forward and backward to see different parts of the output.
Type `h` at the `less` prompt to get help information on what you can do
with it.  Type `q` at the prompt to exit `less`.

Alternatively, the output of the program can be redirected via a pipe to
the `od` command, without using any output file:

> <pre>
> $ bin/huff -c -b 1024 < testfile | od -t x1 | less
> </pre>

## Testing Your Program

Pipes can be used to test your program by compressing a file and immediately
decompressing it:

> <pre>
> $ cat testfile | bin/huff -c -b 1024 | bin/huff -d > outfile
> </pre>

If the program is working properly, the contents of `outfile` should
be identical to those of `testfile`.
It is useful to be able to compare two files to see if they have the same content.
 The `diff` command (use `man diff` to read the manual page) is useful for comparison
of text files, but not particularly useful for comparing binary files such as
compressed data files.  However the `cmp` command can be used to perform a
byte-by-byte comparison of two files, regardless of their content:

> <pre>
> $ cmp file1 file2
> </pre>

If the files have identical content, `cmp` exits silently.
If one file is shorter than the other, but the content is otherwise identical,
`cmp` will report that it has reached `EOF` on the shorter file.
Finally, if the files disagree at some point, `cmp` will report the
offset of the first byte at which the files disagree.
If the `-l` flag is given, `cmp` will report all disagreements between the
two files.

We can take this a step further and run an entire test without using any files:

> <pre>
> $ cmp -l &lt;(cat testfile) &lt;(cat testfile | bin/huff -c -b 1024 | bin/huff -d)
> $ echo $?
> 0
> </pre>

This compares the original file `testfile` with the result of taking that
file and first compressing it using a block size of 1024 and then decompressing it.

> :nerd: `<(...)` is known as process substitution. It is allows the output of the
> program(s) inside the parentheses to appear as a file for the outer program.

> :nerd: `cat` is a command that outputs a file to `stdout`.

Because both files are identical, `cmp` outputs nothing.

## Unit Testing

Unit testing is a part of the development process in which small testable
sections of a program (units) are tested individually to ensure that they are
all functioning properly. This is a very common practice in industry and is
often a requested skill by companies hiring graduates.

> :nerd: Some developers consider testing to be so important that they use a
> work flow called **test driven development**. In TDD, requirements are turned into
> failing unit tests. The goal is then to write code to make these tests pass.

This semester, we will be using a C unit testing framework called
[Criterion](https://github.com/Snaipe/Criterion), which will give you some
exposure to unit testing. We have provided a basic set of test cases for this
assignment.

The provided tests are in the `tests/basecode_tests.c` file. These tests do the
following:

- `validargs_help_test` ensures that `validargs` sets the help bit
correctly when the `-h` flag is passed in.

- `validargs_compress_test` ensures that `validargs` sets the compress-mode bit
correctly when the `-c` flag is passed in.

- `validargs_error_test` ensures that `validargs` returns an error when a blocksize
is specified together with a `-d` flag.

- `help_system_test` uses the `system` syscall to execute your program through
Bash and checks to see that your program returns with `EXIT_SUCCESS`.

### Compiling and Running Tests

When you compile your program with `make`, a `huff_tests` executable will be
created in your `bin` directory alongside the `huff` executable. Running this
executable from the `hw1` directory with the command `bin/huff_tests` will run
the unit tests described above and print the test outputs to `stdout`. To obtain
more information about each test run, you can use the verbose print option:
`bin/huff_tests --verbose=0`.

The tests we have provided are very minimal and are meant as a starting point
for you to learn about Criterion, not to fully test your homework. You may write
your own additional tests in `tests/huff_tests.c`. However, this is not required
for this assignment. Criterion documentation for writing your own tests can be
found [here](http://criterion.readthedocs.io/en/master/).

# Hand-in instructions
**TEST YOUR PROGRAM VIGOROUSLY!**

Make sure that you have implemented all the required functions specifed in `global.h`.

Make sure that you have adhered to the restrictions (no array brackets, no prohibited
header files, no modifications to files that say "DO NOT MODIFY" at the beginning,
no functions other than `main()` in `main.c`) set out in this assignment document.

Make sure your directory tree looks basically like it did when you started
(there could possibly be additional files that you added, but the original organization
should be maintained) and that your homework compiles (you should be sure to try compiling
with both `make clean all` and `make clean debug` because there are certain errors that can
occur one way but not the other).

This homework's tag is: `hw1`

`$ git submit hw1`

> :nerd: When writing your program try to comment as much as possible. Try to
> stay consistent with your formatting. It is much easier for your TA and the
> professor to help you if we can figure out what your code does quickly!

