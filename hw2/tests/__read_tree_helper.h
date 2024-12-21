#include "__file_helpers.h"
#include "global.h"

// structures for holding the reference tree
// for use when testing read_huffman_tree
NODE * t_node_for_symbol[MAX_SYMBOLS];
NODE t_nodes[MAX_SYMBOLS * 2 - 1];

int read_huff_to_test_tree();
int test_read_huff_tree();
