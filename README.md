This course introduces C programming and essential concepts of operating
systems, compilers, concurrency, and performance analysis, focused around
several cross-cutting examples, such as memory management, error handling, and
threaded programming. In this course, operating systems concepts are considered
from the point of view of the application programmer, and the focus is on APIs
for interacting with an operating system.

# HW 1
In this assignment, you will write a command line utility to serialize a tree
of files and directories to a sequence of bytes and to deserialize such a
sequence of bytes to re-create the original tree.  Such a utility could be
used, for example, to "transplant" a tree of files and directories from one
place to another.  The Unix cpio utility is an example of a full-featured
utility of this type. The goal of this homework is to familiarize yourself with
C programming, with a focus on input/output, strings in C, bitwise
manipulations, and the use of pointers.

# HW 2
Your goal will be to debug and try to optimize the huff program, which is code
submitted by a student for HW1 in a previous semester of this course.  The huff
program is a program whose dual functions are: (1) to use Huffman coding to
compress files into (hopefully) shorter versions, and (2) to decompress
compressed files to recover the original version. The code you will work on is
basically what the student submitted, except for a few edits that I made to
make the assignment more interesting and educational 😉.  Although this program
was written by one of the better students in the class, there are always things
that could be criticized about the coding style of any program and you might
want to think about what those might be as you work on the code. Of particular
interest would be ways that the coding style could be improved so as to make
the occurrence of some of the bugs in the code less likely.  You are free to
make any changes you like to the code, and it is not necessary for you to
adhere to the restrictions involving array brackets and the like, which were
imposed on the original code. (You should not, however, put any functions other
than main in the main.c file, as this will affect the Criterion test code.) For
this assignment, you may also make changes to the Makefile (e.g. to change the
options given to gcc). If you find things that make the program very awkward to
work on, you are welcome to rewrite them, but it is not really in the sprit of
the assignment to just rewrite the whole thing (and I am guessing that you
probably don't want to do that anyway).  Instead, try to fix bugs and make
performance improvements within the context of the existing code.

# HW 3
You will create an allocator for the x86-64 architecture with the following features:

- Free lists segregated by size class, using first-fit policy within each size class.
- Immediate coalescing of large blocks on free with adjacent free blocks.
- Boundary tags to support efficient coalescing, with footer optimization that allows
    footers to be omitted from allocated blocks.
- Block splitting without creating splinters.
- Allocated blocks aligned to "quad memory row" (32-byte) boundaries.
- Free lists maintained using **last in first out (LIFO)** discipline.
- Use of a prologue and epilogue to avoid edge cases at the end of the heap.
- "Wilderness preservation" heuristic, to avoid unnecessary growing of the heap.

You will implement your own versions of the **malloc**, **realloc**, **free**,
and **memalign** functions.

You will use existing Criterion unit tests and write your own to help debug
your implementation.


# HW 4
The goal of this assignment is to become familiar with low-level Unix/POSIX
system calls related to processes, signal handling, files, and I/O redirection.
You will implement a program, called cook, that behaves like a simplified
version of make, with the ability to run jobs in parallel up to a specified
maximum.

# HW 5
"PBX" is a simple implementation of a server that simulates a PBX telephone
system.  A PBX is a private telephone exchange that is used within a business
or other organization to allow calls to be placed between telephone units (TUs)
attached to the system, without having to route those calls over the public
telephone network.
