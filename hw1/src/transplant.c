#include "global.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

#define START_OF_TRANSMISSION 0
#define END_OF_TRANSMISSION 1
#define START_OF_DIRECTORY 2
#define END_OF_DIRECTORY 3
#define DIRECTORY_ENTRY 4
#define FILE_DATA 5

#define HEADER_SIZE 16
#define METADATA_SIZE 12

/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 *
 * Variables to hold the pathname of the current file or directory
 * as well as other data have been pre-declared for you in global.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

int stringLength (char *str) {
    int count = 0;
    while (*str != '\0') {
        count++;
        str++;
    }
    return count;
}

void copyString (char *dest, char* src) {
    while (*src != '\0') {
        *dest = *src;
        src++;
        dest++;
    }
    *dest = '\0';
}

int sameString (char *s1, char *s2) {
    while (*s1 != '\0' && *s2 != '\0') {
        if (*s1 != *s2)
            return 0;
        s1++;
        s2++;
    }
    return !(*s1 ^ *s2);
}

void write_bytes (uint64_t val, size_t numBytes) {
    for (int i = 0; i < numBytes; i++)
        putchar((val >> ((numBytes-1 - i) * 8)) & 0xFF);
}

uint64_t read_bytes (size_t numBytes) {
    uint64_t result = 0;

    for (size_t i = 0; i < numBytes; i++) {
        result = (result << 8) | getchar();
    }

    return result;
}

int read_name (int length) {
    if (length > NAME_MAX)
        return -1;

    char *index = name_buf;
    for (int i = 0; i < length; i++, index++)
        *index = getchar();
    *index = '\0';
    return 0;
}

int write_header (uint8_t type, uint32_t depth, uint64_t size) {
    putchar(0x0c);
    putchar(0x0d);
    putchar(0xed);
    putchar(type);
    write_bytes(depth, sizeof(depth));
    write_bytes(size, sizeof(size));
    return 0;
}

void directory_entry (int depth, mode_t mode, off_t size, char *name) {
    int total_size = HEADER_SIZE + METADATA_SIZE + stringLength(name);
    write_header(DIRECTORY_ENTRY, depth, total_size);
    write_bytes(mode, 4);
    write_bytes(size, 8);
    while (*name != 0) {
        putchar(*name);
        name++;
    }
}

int already_exists (char *name) {
    struct stat buf;
    return !stat(name, &buf);
}

/*
 * @brief  Initialize path_buf to a specified base path.
 * @details  This function copies its null-terminated argument string into
 * path_buf, including its terminating null byte.
 * The function fails if the argument string, including the terminating
 * null byte, is longer than the size of path_buf.  The path_length variable
 * is set to the length of the string in path_buf, not including the terminating
 * null byte.
 *
 * @param  Pathname to be copied into path_buf.
 * @return 0 on success, -1 in case of error
 */
int path_init(char *name) {
    // TODO: absolute vs relative paths
    int length = stringLength(name);
    if (length >= PATH_MAX)
        return -1;
    copyString(path_buf, name);
    path_length = length;
    return 0;
}

/*
 * @brief  Append an additional component to the end of the pathname in path_buf.
 * @details  This function assumes that path_buf has been initialized to a valid
 * string.  It appends to the existing string the path separator character '/',
 * followed by the string given as argument, including its terminating null byte.
 * The length of the new string, including the terminating null byte, must be
 * no more than the size of path_buf.  The variable path_length is updated to
 * remain consistent with the length of the string in path_buf.
 *
 * @param  The string to be appended to the path in path_buf.  The string must
 * not contain any occurrences of the path separator character '/'.
 * @return 0 in case of success, -1 otherwise.
 */
int path_push(char *name) {
    int length = stringLength(name);
    if (path_length + length >= PATH_MAX)
        return -1;
    char *end = path_buf;
    while (*end != '\0')
        end++;
    *end = '/';
    end++;
    copyString(end, name);
    path_length = stringLength(path_buf);
    return 0;
}

/*
 * @brief  Remove the last component from the end of the pathname.
 * @details  This function assumes that path_buf contains a non-empty string.
 * It removes the suffix of this string that starts at the last occurrence
 * of the path separator character '/'.  If there is no such occurrence,
 * then the entire string is removed, leaving an empty string in path_buf.
 * The variable path_length is updated to remain consistent with the length
 * of the string in path_buf.  The function fails if path_buf is originally
 * empty, so that there is no path component to be removed.
 *
 * @return 0 in case of success, -1 otherwise.
 */
int path_pop() {
    if (!*path_buf)
        return -1;

    char *cur = path_buf;
    char *lastDelim = cur;

    while (*cur != '\0') {
        if (*cur == '/')
            lastDelim = cur;
        cur++;
    }
    *lastDelim = '\0';
    path_length = stringLength(path_buf);
    return 0;
}

/*
 * @brief Deserialize directory contents into an existing directory.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory.  It reads (from the standard input) a sequence of DIRECTORY_ENTRY
 * records bracketed by a START_OF_DIRECTORY and END_OF_DIRECTORY record at the
 * same depth and it recreates the entries, leaving the deserialized files and
 * directories within the directory named by path_buf.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * each of the records processed.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including depth fields in the records read that do not match the
 * expected value, the records to be processed to not being with START_OF_DIRECTORY
 * or end with END_OF_DIRECTORY, or an I/O error occurs either while reading
 * the records from the standard input or in creating deserialized files and
 * directories.
 */
int deserialize_directory(int depth) {
    mkdir(path_buf, 0700);
    return 0;
}

/*
 * @brief Deserialize the contents of a single file.
 * @details  This function assumes that path_buf contains the name of a file
 * to be deserialized.  The file must not already exist, unless the ``clobber''
 * bit is set in the global_options variable.  It reads (from the standard input)
 * a single FILE_DATA record containing the file content and it recreates the file
 * from the content.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * the FILE_DATA record.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including a depth field in the FILE_DATA record that does not match
 * the expected value, the record read is not a FILE_DATA record, the file to
 * be created already exists, or an I/O error occurs either while reading
 * the FILE_DATA record from the standard input or while re-creating the
 * deserialized file.
 */
int deserialize_file(int depth) {
    FILE *file = fopen(path_buf, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file '%s'\n", path_buf);
        return -1;
    }
    fclose(file);
    return 0;
}


/*
 * @brief  Serialize the contents of a directory as a sequence of records written
 * to the standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory to be serialized.  It serializes the contents of that directory as a
 * sequence of records that begins with a START_OF_DIRECTORY record, ends with an
 * END_OF_DIRECTORY record, and with the intervening records all of type DIRECTORY_ENTRY.
 *
 * @param depth  The value of the depth field that is expected to occur in the
 * START_OF_DIRECTORY, DIRECTORY_ENTRY, and END_OF_DIRECTORY records processed.
 * Note that this depth pertains only to the "top-level" records in the sequence:
 * DIRECTORY_ENTRY records may be recursively followed by similar sequence of
 * records describing sub-directories at a greater depth.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open files, failure to traverse directories, and I/O errors
 * that occur while reading file content and writing to standard output.
 */
int serialize_directory(int depth) {

    write_header(START_OF_DIRECTORY, depth, HEADER_SIZE);

    DIR *dir = opendir(path_buf);

    struct dirent *de = readdir(dir);
    while (de != NULL) {
        if (sameString(de->d_name, ".") || sameString(de->d_name, "..")) {
            de = readdir(dir);
            continue;
        }

        path_push(de->d_name);

        struct stat stat_buf;
        stat(path_buf, &stat_buf); // TODO: error checking

        directory_entry(depth, stat_buf.st_mode, stat_buf.st_size, de->d_name);

        if (S_ISDIR(stat_buf.st_mode))
            serialize_directory(depth + 1); // TODO: error checking
        else if (S_ISREG(stat_buf.st_mode))
            serialize_file(depth, stat_buf.st_size); // TODO: error checking

        de = readdir(dir);
    }
    closedir(dir);

    write_header(END_OF_DIRECTORY, depth, HEADER_SIZE);

    path_pop();
    return 0;
}

/*
 * @brief  Serialize the contents of a file as a single record written to the
 * standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * file to be serialized.  It serializes the contents of that file as a single
 * FILE_DATA record emitted to the standard output.
 *
 * @param depth  The value to be used in the depth field of the FILE_DATA record.
 * @param size  The number of bytes of data in the file to be serialized.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open the file, too many or not enough data bytes read
 * from the file, and I/O errors reading the file data or writing to standard output.
 */
int serialize_file(int depth, off_t size) {

    write_header(FILE_DATA, depth, HEADER_SIZE + size);

    FILE *f = fopen(path_buf, "r"); // TODO: error checking

    int c;
    while ((c = fgetc(f)) != EOF) {
        putchar(c);
    }

    fclose(f);
    path_pop();
    return 0;
}

/**
 * @brief Serializes a tree of files and directories, writes
 * serialized data to standard output.
 * @details This function assumes path_buf has been initialized with the pathname
 * of a directory whose contents are to be serialized.  It traverses the tree of
 * files and directories contained in this directory (not including the directory
 * itself) and it emits on the standard output a sequence of bytes from which the
 * tree can be reconstructed.  Options that modify the behavior are obtained from
 * the global_options variable.
 *
 * @return 0 if serialization completes without error, -1 if an error occurs.
 */
int serialize() {
    if (!already_exists(path_buf)) {
        fprintf(stderr, "Error: No such path: %s\n", path_buf);
        return -1;
    }

    write_header(START_OF_TRANSMISSION, 0, HEADER_SIZE);

    struct stat input_path;
    stat(path_buf, &input_path); // TODO: error checking

    if (S_ISDIR(input_path.st_mode))
        serialize_directory(1); // TODO: error checking
    else // ie, path is not a directory
        return -1;

    write_header(END_OF_TRANSMISSION, 0, HEADER_SIZE);

    fflush(stdout);
    return 0;
}

/**
 * @brief Reads serialized data from the standard input and reconstructs from it
 * a tree of files and directories.
 * @details  This function assumes path_buf has been initialized with the pathname
 * of a directory into which a tree of files and directories is to be placed.
 * If the directory does not already exist, it is created.  The function then reads
 * from from the standard input a sequence of bytes that represent a serialized tree
 * of files and directories in the format written by serialize() and it reconstructs
 * the tree within the specified directory.  Options that modify the behavior are
 * obtained from the global_options variable.
 *
 * @return 0 if deserialization completes without error, -1 if an error occurs.
 */
int deserialize() {
    int clobber = global_options & 0b1000;

    if (!sameString(path_buf, ".")) {
        if (!clobber && already_exists(path_buf)) {
            fprintf(stderr, "Error: Already exists: %s\n", path_buf);
            return -1;
        }
        mkdir(path_buf, 0700);
    }

    int currentDepth = 0;

    off_t deSize;

    char c;
    while ((c = getchar()) != EOF) {
        if (c != 0x0c || getchar() != 0x0d || getchar() != 0xed) {
	    fprintf(stderr, "Error: magic sequence not found");
            return -1;
	}

        uint8_t type = read_bytes(1);
        uint32_t depth = read_bytes(4);
        uint64_t size = read_bytes(8);

        if (depth != currentDepth) {
            fprintf(stderr, "Depth error: expected %d, got %u\n", currentDepth, depth);
            return -1;
        }

        switch (type) {
            case START_OF_TRANSMISSION:
                if (depth != 0) {
                    fprintf(stderr, "Start of Transmission depth error: expected 0, got %u\n", depth);
                    return -1;
                }
                if (size != 16) {
                    fprintf(stderr, "Error: unexpected size\n");
                    return -1;
                }
                currentDepth++;
                break;

            case END_OF_TRANSMISSION:
                if (currentDepth != 0) {
                    fprintf(stderr, "End of Transmission depth error: expected 0, got %u\n", depth);
                    return -1;
                }
                if (size != 16) {
                    fprintf(stderr, "Error: unexpected size\n");
                    return -1;
                }
                break;

            case START_OF_DIRECTORY:
                if (size != 16) {
                    fprintf(stderr, "Error: unexpected size\n");
                    return -1;
                }
                break;

            case END_OF_DIRECTORY:
                if (size != 16) {
                    fprintf(stderr, "Error: unexpected size\n");
                    return -1;
                }
                currentDepth--;
                path_pop();
                break;

            case DIRECTORY_ENTRY:
                mode_t mode = read_bytes(4);
                deSize = read_bytes(8);

                int nameLength = size - (HEADER_SIZE + METADATA_SIZE);
                read_name(nameLength);

                path_push(name_buf);

                if (!clobber && already_exists(path_buf)) {
                    fprintf(stderr, "Error: Already exists: %s\n", path_buf);
                    return -1;
                }

                if (S_ISDIR(mode)) {
                    if (deserialize_directory(depth)) {
                        fprintf(stderr, "Error deserializing directory%s\n", path_buf);
                        return -1;
                    }
                    chmod(path_buf, mode & 0777);
                    currentDepth++;
                }

                else if (S_ISREG(mode)) {
                    if (deserialize_file(depth)) {
                        fprintf(stderr, "Error deserializing file: %s\n", path_buf);
                        return -1;
                    }
                    chmod(path_buf, mode & 0777);
                }

                else
                    return -1;
                break;

            case FILE_DATA:
                off_t fileSize = size - HEADER_SIZE;
                if (fileSize != deSize)
                    return -1;

                FILE *file = fopen(path_buf, "w");
                if (file == NULL) {
                    fprintf(stderr, "Error opening file '%s'\n", path_buf);
                    return -1;
                }

                for (off_t i = 0; i < fileSize; i++) {
                    fputc(getchar(), file);
                }
                fflush(file);
                fclose(file);
                path_pop();
                break;

            default:
                fprintf(stderr, "Error: unrecognized type");
                return -1;
        }
    }
    return 0;
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv) {
    int sMode = 0;
    int dMode = 0;

    if (argc < 2)
        return -1;

    for (int i = 1; i < argc; i++) {
        char *arg = *(argv + i);

        if (*arg == '-') {
            char c = *(arg + 1);

            if (c == 'h') {
                if (i != 1)
                    return -1;
                global_options |= 0b0001;
                return 0;
            }

            if (c == 's') {
                if (dMode)
                    return -1;
                sMode = 1;
                global_options |= 0b0010;
            }

            else if (c == 'd') {
                if (sMode)
                    return -1;
                dMode = 1;
                global_options |= 0b0100;
            }

            else if (c == 'c')
            {
                if (!dMode)
                    return -1;
                global_options |= 0b1000;
            }

            else if (c == 'p')
            {
                if (!(sMode || dMode))
                    return -1;
                i++;
                char *path = *(argv + i);
                int len = stringLength(path);
                if (!path || len >= NAME_MAX)
                    return -1;
                char *lastChar = (path + len - 1);
                if (*lastChar == '/')
                    *lastChar = '\0';
                copyString(name_buf, path);
            }

            else
                return -1;
        }

        else
            return -1;
    }

    return 0;
}
