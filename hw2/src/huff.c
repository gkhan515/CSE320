#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "huff.h"
#include "debug.h"

//helper method to get a sequence of n bits where 0 = leaf and 1 = internal node
//since we will be recursively calling, we will use a pointer to out and count
//to keep track of the actual value
static void postorder(NODE* root, unsigned char* out, int* count) {
    //in postorder traversal, we do left, right, then root
    if(root->left != NULL)
        postorder(root->left, out, count);

    if(root->right != NULL)
        postorder(root->right, out, count);

    //if leaf, shift char left 1 and add 0, then increment count
    if(root->left == NULL && root->right == NULL)   {
        *out = *out << 1;
        *out += 0;
        (*count)++;
    }
    //else, shift char left 1 and add 1, then increment count
    else    {
        *out = *out << 1;
        *out += 1;
        (*count)++;
    }

    //if count = 8, out is filled up so we output it and reset
    if(*count == 8)  {
        fputc(*out, stdout);
        fflush(stdout);
        *out = 0;
        *count = 0;
    }
    return;
}

//helper method to postorder the tree and output the symbols left to right
static void symboltraversal(NODE* root) {
    //recursive call on left and right subtrees
    if(root->left != NULL)
        symboltraversal(root->left);
    
    if(root->right != NULL)
        symboltraversal(root->right);

    //if leaf, we output its symbol
    if(root->left == NULL && root->right == NULL)   {
        //if symbol is 256, it is end block so output FF 00
        if(root->symbol == 256) {
            fputc(0xFF, stdout);
            fputc(0x00, stdout);
            fflush(stdout);
        }
        //if symbol is 0xFF, we out put 0xFF and any byte other than 0x00. I will
        //output 0xFF twice
        else if(root->symbol == 255)    {
            fputc(0xFF, stdout);
            fputc(0xFF, stdout);
            fflush(stdout);
        }
        //else we output its symbol normally
        else    {
            fputc(root->symbol, stdout);
            fflush(stdout);
        }
    }
    return;
}

/**
 * @brief Emits a description of the Huffman tree used to compress the current block.
 * @details This function emits, to the standard output, a description of the
 * Huffman tree used to compress the current block.  Refer to the assignment handout
 * for a detailed specification of the format of this description.
 */
void emit_huffman_tree() {
    //output the number of nodes, given as a 2 byte sequence in big-endian order
    unsigned char msb;
    unsigned char lsb;
    msb = (num_nodes >> 8) & 0xFF;
    lsb = num_nodes & 0xFF;
    fputc(msb, stdout);
    fputc(lsb, stdout);
    fflush(stdout);

    //a sequence of n bits from a postorder traversal (0 = leaf, 1 = internal node)
    unsigned char out = 0;
    int count = 0;
    postorder(nodes, &out, &count);
    //after finishing the recursive call, we pad out with 0s if count not 0
    if(count != 0)  {
        out = out << (8 - count);
        fputc(out, stdout);
        fflush(stdout);
    }

    //a sequence of bytes that gives the values of the symbols at the (n+1)/2
    //leaves of the tree. each byte is 1 symbol except FF 00 which is end block
    symboltraversal((nodes+0));
    
    return;
}

//helper method to traverse tree and set its node's parents
static void set_parents(NODE* root)   {
    //if left is not null, set left's parent as root and recursive call set_parents
    if(root->left != NULL)  {
        root->left->parent = root;
        set_parents(root->left);
    }
    //if right is not null, set right's parent as root and recursive call set_parents

    if(root->right != NULL)  {
        root->right->parent = root;
        set_parents(root->right);
    }
    return;
}

//helper method to traverse tree and set node_for_symbol with its leaves in postorder
static void postordertrackleaf(NODE* root, int* add)    {
    //postorder traversal goes left, right, then root
    if(root->left != NULL)
        postordertrackleaf(root->left, add);
    if(root->right != NULL)
        postordertrackleaf(root->right, add);
    
    //if we are at leaf we put it into the node_for_symbol array
    if(root->left == NULL && root->right == NULL)   {
        *(node_for_symbol+*add) = root;
        (*add)++;
    }
}

//helper method to find a node for a symbol
static NODE *findnode(int c) {
    for(int i = 0; i < num_nodes; i++) {
	if((nodes+i)->symbol == c)
	    return nodes+i;
    }
    return NULL;
}

/**
 * @brief Reads a description of a Huffman tree and reconstructs the tree from
 * the description.
 * @details  This function reads, from the standard input, the description of a
 * Huffman tree in the format produced by emit_huffman_tree(), and it reconstructs
 * the tree from the description.  Refer to the assignment handout for a specification
 * of the format for this description, and a discussion of how the tree can be
 * reconstructed from it.
 *
 * @return 0 if the tree is read and reconstructed without error, 1 if EOF is
 * encountered at the start of a block, otherwise -1 if an error occurs.
 */
int read_huffman_tree() {
    //reading 2 bytes first
    unsigned char* currblockptr = current_block;
    for(int i = 0; i < 2; i++)  {
        *currblockptr++ = fgetc(stdin);
        // return 1 for empty tree, -1 for single byte
        if(feof(stdin))
            return i==0 ? 1:-1;
        if (ferror(stdin))
            return -1;
    }
    //the first 2 bytes read will be num_nodes
    num_nodes = *current_block << 8;
    num_nodes += *(current_block+1);

    if (num_nodes < 3)
        return -1;

    int n = 0;
    //the number of bytes representing the tree
    if(num_nodes % 8 == 0)
        n = num_nodes/8;
    else
        n = num_nodes/8 + 1;
    //read the bytes representing the tree
    for(int i = 0; i < n; i++)  {
        *currblockptr++ = fgetc(stdin);
        //if end of file or error, return -1
        if(feof(stdin) || ferror(stdin))
            return -1;
    }
    
    //the next num_nodes/8 bytes will be the postorder traversal bit sequence
    int position = num_nodes - 1;
    NODE* stackptr = nodes;
    int last = num_nodes % 8; 
    currblockptr = (current_block+2);   
    for(int i = 0; i < n; i++)    {
        unsigned char a = *currblockptr++;
        //inner loop to use all 8 bits of the byte
        for(int count = 0; count < 8; count++)  {
            //if this is the last byte and count = remainder of num_nodes%8
            //then break as the rest of the byte is padding
            if((i == n - 1) && count == last)
                break;
            
            //if the bit is a 0 (leaf), create a new node and push onto stack
            if((a & 0x80) == 0)    {
                NODE q;
                q.left = NULL;
                q.right = NULL;
                q.parent = NULL;
                *stackptr++ = q;
            }
            //else the bit is a 1 (internal node), pop 2 nodes and place them at
            //the back of the nodes array, and create a new node with those
            //nodes as its children
            else    {
                if (stackptr - nodes < 2) // no_leaf_nodes
                    return -1;
                stackptr--;
                NODE R = *stackptr;
                stackptr--;
                NODE L = *stackptr;
                //we place them at the back of the nodes array
                *(nodes+position) = R;
                position--;
                *(nodes+position) = L;
                position--;

                NODE P;
                P.left = (nodes+position+1);
                P.right = (nodes+position+2);
                P.parent = NULL;
                *stackptr++ = P;
            }
            a = a << 1;
        }
    }
    
    //we traverse the tree post order and put leaf pointers in the node_for_symbol array
    int zzz = 0;
    postordertrackleaf(nodes, &zzz);
    //we are uncertain of the bytes representing leaves because end block and 0xFF
    //both are represented by 2 bytes. Thus we fgetc as we go
    for(int i = 0; i < zzz; i++)    {
        unsigned char asd = fgetc(stdin);
        if(feof(stdin) || ferror(stdin))
            return -1;
        //if next char is 0xFF
        if(asd == 0xFF) {
            //get another char
            unsigned char qwe = fgetc(stdin);
            if(feof(stdin) || ferror(stdin))
                return -1;
            //if 0xFF is followed by 0x00 it is the end block symbol
            if(qwe == 0x00)
                (*(node_for_symbol+i))->symbol = 256;
            //if it is followed by anything else, it is 0xFF
            else    {
                (*(node_for_symbol+i))->symbol = 0xFF;
            }
        }
        //else the next leaf's symbol is the gotten char
        else
            (*(node_for_symbol+i))->symbol = asd;
    }

    //then we set the parents in the tree
    set_parents((nodes+0));
    return 0;
}

//helper method to construct a histogram from the block
//also puts symbol nodes into the nodes_for_symbol array
static void construct_histogram(int blocksize)  {
    //we will use num_nodes to track symbol count
    num_nodes = 0;
    for(int i = 0; i < blocksize; i++) {
        //used to keep track of whether it is a new symbol
        int newsymbol = 1;
        //loops through current symbols and if there is, increment its weight
        for(int j = 0; j < num_nodes; j++)    {
            if((nodes+j)->symbol == *(current_block+i))  {
                ((nodes+j)->weight)++;
                newsymbol = 0;
                break;
            }
        }
        //if newsymbol = 1 then we create a new node for the symbol
        //we also put a pointer to that node in the node_for_symbol array
        if(newsymbol == 1)  {
            NODE a;
            a.symbol = *(current_block+i);
            a.parent = NULL;
            a.left = NULL;
            a.right = NULL;
            a.weight = 1;
            *(nodes+num_nodes) = a;
            num_nodes++;
        }
    }
    //we make an node to represent end of file
    NODE a;
    a.symbol = 256;
    a.parent = NULL;
    a.left = NULL;
    a.right = NULL;
    a.weight = 0;
    *(nodes+num_nodes) = a;
    num_nodes++;

    return;
}

//helper method to construct a huffman tree from the histogram
static void construct_huffmantree()    {
    //gets the total number of nodes using our leaves
    int totalnodes = num_nodes * 2 - 1;
    //position to place minimum nodes for the huffman tree
    int position = totalnodes - 1;
    //current number of nodes whose position is not decided in the huffman tree
    int currunsorted = num_nodes;
    //while there is more than 1 unsorted node
    while(currunsorted > 1) {
        //minimum weight nodes
        int min1 = nodes->weight;
        int min2 = (nodes+1)->weight;
        int node1 = 0;
        int node2 = 1;
        //loop through current undecided nodes
        for(int i = 2; i < currunsorted; i++)   {
            //if current node's weight >= both min1 and min2 we continue
            if((nodes+i)->weight >= min1 && (nodes+i)->weight >= min2)
                continue;
            //if the current node is less than both min1 and min2
            else if((nodes+i)->weight < min1 && (nodes+i)->weight < min2)    {
                if(min1 < min2) {
                    min2 = (nodes+i)->weight;
                    node2 = i;
                }
                else    {
                    min1 = (nodes+i)->weight;
                    node1 = i;
                }
            }
            //if current node < min1 but >= min2, set min1 to current node's weight
            else if((nodes+i)->weight < min1 && (nodes+i)->weight >= min2)  {
                min1 = (nodes+i)->weight;
                node1 = i;
            }
            //else set min2 to current node's weight
            else    {
                min2 = (nodes+i)->weight;
                node2 = i;
            }
        }
        //move the minimum nodes the "high end" region of the array
        *(nodes+position) = *(nodes+node2);
        position--;
        *(nodes+position) = *(nodes+node1);
        position--;

        //create a new internal node with the 2 minimum nodes as its children
        NODE a;
        a.parent = NULL;
        a.symbol = -1;
        a.left = (nodes+position+1);
        a.right = (nodes+position+2);
        //set a's weight to the sum of its children
        a.weight = (nodes+position+1)->weight + (nodes+position+2)->weight;


        //insert a to the section of unsorted nodes taking the lefter node's position
        int max = 0;
        if(node1 < node2)   {
            *(nodes+node1) = a;
            max = node2;
        }
        else    {
            *(nodes+node2) = a;
            max = node1;
        }
        //decrement currunsorted
        currunsorted--;
        //if currunsorted is not node1 or node2, we shift the leftmost node to node1 or node2
        if(currunsorted != node1 && currunsorted != node2)
            *(nodes+max) = *(nodes+currunsorted);

    }
    //set num_nodes to the current total nodes
    num_nodes = totalnodes;
    return;
}

//helper method to convert the block of data to compressed data and output
static void outputcompressed(int blocksize)    {
    //out is 1 byte to be printed while count keeps track of when out reaches 8 bits
    unsigned char out = 0;
    int count = 0;
    NODE* a = NULL;
    //compression to be repeated for every element in buffer
    for(int i = 0; i < blocksize+1; i++) {
        //if i == blocksize, we set a as end block pointer
        if(i == blocksize)
            a = findnode(256);
        //else we set a normally
        else
            a = findnode(*(current_block+i));
        //while not at root
        while(a != NULL && a->parent != NULL) {
            //if a is a left child set its parents weight as 0
            if(a->parent->left != NULL && a->parent->left == a)    {
                a = a->parent;
                a->weight = 0;
            }
            //else if a is a right child set its parents weight as 1
            else    {
                a = a->parent;
                a->weight = 1;
            }
        }
        //while not at leaf
        while(a && a->left != NULL && a->right != NULL)  {
            //we put the bits into char out one by one and increment count
            out = out << 1;
            out += a->weight;
            count++;
            //if count = 8 then we filled all 8 bits of out so we output it and
            //reset count to 0
            if(count == 8)  {
                fputc(out, stdout);
                fflush(stdout);
                count = 0;
            }

            //depending on whether weight is 0 or 1, set a to its left/right child
            if(a->weight == 0)
                a = a->left;
            else
                a = a->right;
        }
    }

    //we pad the last byte with 0s if it hasn't filled all 8 bits
    out = out << (8 - count);
    fputc(out, stdout);
    fflush(stdout);

    return;
}

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
int compress_block() {
    //get blocksize from global_options upper 16 bits
    int blocksize = ((unsigned int) global_options >> 16) + 1;
    int read = 0;
    //if EOF at beginning of block return error
    if(feof(stdin))
        return -1;
    //reading 1 block of data from stdin to array current_block
    char c;
    for(; read < blocksize; read++)  {
        //get char then put in array block_size
        c = fgetc(stdin);
        //if end of file, exit loop
        if(feof(stdin)) {
            break;
        }
        //if an error occurs, return -1
        else if(ferror(stdin))  {
            return -1;
        }
        *(current_block+read) = c;
    }

    //read will be the same as block_size or end block
    blocksize = read;
    //if blocksize <= 0 it is an error so return -1
    if(blocksize <= 0)
        return -1;

    //we use blocksize to keep track of EOF or end of block
    //constructs a histogram from the buffer
    construct_histogram(blocksize);
    //uses histogram to construct huffmantree
    construct_huffmantree();
    //traverses through huffman tree and sets parents
    set_parents((nodes+0));
    //sets node_for_symbol array with leaves of huffman tree
    for(int i = 0; i < num_nodes; i++)  {
        //if symbol is not a -1 (internal node)
        if((nodes+i)->symbol != -1)
            *(node_for_symbol+((nodes+i)->symbol)) = nodes+i;
    }
    //emits description of tree
    emit_huffman_tree();
    //compress the data and output it
    outputcompressed(blocksize);

    return 0;
}

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
int decompress_block() {
    //read huffman tree
    int q = read_huffman_tree();
    // if return is not 0 then it is an error
    // if(q == -1 || q == 1)
    //     return -1;
    if (q != 0)
        return -1;

    unsigned char c = fgetc(stdin);
    if(feof(stdin) || ferror(stdin))
        return -1;
    int count = 0;
    NODE *a = nodes;
    if (!a->left || !a->right)
        return -1;
    int bit;
    //we do loop until break or return error
    while(1) {
        //NODE pointer to root of tree
        a = nodes;
        //while a is not a leaf
        while(a->left != NULL && a->right != NULL)  {
            //c & 10000000 will test if the most significant bit is 1 or 0
            bit = c & 0x80;
            //then we shift c left 1 and increment count
            c = c << 1;
            count++;
            //if count = 8 then we have used all 8 bits of the byte
            //so we get the next byte and reset count
            if(count == 8)  {
                //since we are unsure of where end block is, we must get the input one
                //by one instead of a whole block at once
                c = fgetc(stdin);
                if(feof(stdin) || ferror(stdin))
                    return -1;
                count = 0;
            }

            //if bit = 0, then the msb was 0 so we go left else right for 1
            if(bit == 0)
                a = a->left;
            else
                a = a->right;
        }
        //after the inner loop ends, we are at a leaf
        //if the symbol represents EOF, then we break
        if(a->symbol == 256)
            break;

        //we output the symbol
        fputc(a->symbol, stdout);
        // fflush(stdout);
    }

    fflush(stdout);
    return 0;
}

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
int compress() {
    //while not at EOF, call compress_block
    int error = 0;
    int compressed = 0;
    while(feof(stdin) == 0)  {
        error = compress_block();
        //if compress_block returns -1 and we did not compress any blocks return -1;
        if(error && compressed == 0)
            return 0;
        compressed++;
    }
    //if error occurs, return -1
    if(ferror(stdin))
        return -1;

    return 0;
}

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
int decompress() {
    //while not at EOF, call decompress_block
    int error = 0;
    int decompressed = 0;
    while(feof(stdin) == 0)  {
        error = decompress_block();
        //if decompress_block returns -1 and we have no decompressed blocks, return -1
        if (error)
            return 0;
        decompressed++;
    }
    //if error return -1
    if(ferror(stdin))
        return -1;

    return 0;
}

//helper method strequal to compare if 2 strings are equal
//returns 0 if not equal else 1 if equal
static int strequal(char* first, char* second) {
    while(*first != '\0' || *second != '\0')    {
        if(*first != *second)
            return 0;
        first++;
        second++;
    }
    return 1;
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
int validargs(int argc, char **argv)
{
    //only argument is file name, return -1
    if(argc == 1)   {
        return -1;
    }
    
    //if the first argument is -h then we set global_options to 0x1 and return
    //with 0
    if(strequal(*(argv+1), "-h"))  {
        global_options = 1;
        return 0;
    }

    //if there are 2 arguments, one is the function name and the other is c or d
    if(argc == 2)   {
        //for compression, we set global_options as 2 with the default ffff0000
        //size and return with success
        if(strequal(*(argv+1), "-c"))   {
            global_options = 0xffff0002;
            return 0;
        }
        //for decompression, we set global_options as 4 with the default ffff0000
        //size and returnw ith success
        else if(strequal(*(argv+1), "-d"))  {
            global_options = 0xffff0004;
            return 0;
        }
        //if the argument is neither -c or -d then return with -1
        else
            return -1;
    }

    //if there are 4 arguments the last 3 should be -c, -b, then a number
    if(argc == 4)   {
        //if the 2nd argument is -c and the 3rd argument is -b
        if(strequal(*(argv+1), "-c") && strequal(*(argv+2), "-b"))  {
            //while the 4th argument is not null
            int blocksize = 0;
            while(**(argv+3) != '\0') {
                // error on non-numeric character
                if (!(**(argv+3) >= '0' && **(argv+3) <= '9'))
                    return -1;

                //multiply blocksize by 10 and add the new number to it by
                //by subtracting the ascii value of '0' from it
                blocksize = blocksize * 10 + **(argv+3) - '0';
                (*(argv+3))++;
            }

            //if blocksize is not within bounds, return -1
            if(blocksize < MIN_BLOCK_SIZE || blocksize > MAX_BLOCK_SIZE)
                return -1;
            //else set global_options's upper 16 bits as blocksize - 1
            //then increment by 2 representing compression and return 0
            else    {
                global_options = (blocksize - 1) << 16;
                global_options += 2;
                return 0;
            }
        }
    }

    //if no options match, return -1
    return -1;
}
