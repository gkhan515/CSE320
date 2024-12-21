#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "test_common.h"
#include "__read_tree_helper.h"

#define ROUNDUP(num) (((num + 7) / 8) * 8)
#define HUFFTREEDESC (2 + ROUNDUP(num_nodes) / 8 + (num_nodes+1)/2)
#define BLOCKSIZE (((global_options >> 16)& 0xffff)+ 1)

// TODO: This really ought to be dynamically allocated.
static char cmd[512];
short symbols[MAX_SYMBOLS];
int num_of_symbols;

int assert_isomorphism_test();
int check_nodes_tree_test(NODE*);

// Test proper return on gettysburg.txt
Test(compress_test_suite, compress_test0, .timeout = 5){
	open_input_file("./tests/rsrc/c_test0.in");
	FILE *fout = open_output_file("./test_output/compress_test_suite/c_test0.out");

	global_options = (MAX_BLOCK_SIZE - 1) << 16;
	int ret = compress();
	int exp_ret = EXIT_SUCCESS;
	cr_assert_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	fflush(stdout);
	if (fclose(fout) < 0) fprintf(stderr, "Couldn't close output\n");
}


// Compressing a file smaller than blocksize
Test(compress_test_suite, compress_block_test0, .timeout = 5){
	char * in = "./tests/rsrc/c_block_test0.in";
	char * out = "./test_output/compress_test_suite/c_block_test0.out";

	FILE *fin = open_input_file(in);
	FILE *fout = open_output_file(out);
	
	global_options = (MAX_BLOCK_SIZE - 1) << 16;

	int ret = compress_block();
	int exp_ret = EXIT_SUCCESS;
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);
	
	fflush(stdout);
	if (fclose(fin) < 0 || fclose(fout) < 0){
		fprintf(stderr, "Couldn't close file");
		return;
	}

	sprintf(cmd, "%s bash -c 'cmp -n %d %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS,  BLOCKSIZE, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// compress_block a file larger than blocksize
Test(compress_test_suite, compress_block_test1, .timeout = 5){
	char * in = "./tests/rsrc/c_block_test1.in";
	char * out = "./test_output/compress_test_suite/c_block_test1.out";

	FILE* fin = open_input_file(in);
	FILE* fout = open_output_file(out);
	global_options = (1024 - 1) << 16;

	int ret  = compress_block();
	int exp_ret = EXIT_SUCCESS;
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);
	
	fflush(stdout);
	if (fclose(fin) < 0 || fclose(fout) < 0){
		fprintf(stderr, "Couldn't close file");
		return;
	}

	sprintf(cmd, "%s bash -c 'cmp -n %d %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS,  BLOCKSIZE, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// compress_block on empty file
Test(compress_test_suite, compress_block_test2, .timeout = 5){
	char * in = "./tests/rsrc/c_empty_test.in";
	char * out = "./test_output/compress_test_suite/c_empty_test.out";

	FILE *fin = open_input_file(in);
	FILE *fout = open_output_file(out);
	global_options = (MAX_BLOCK_SIZE - 1) << 16;

	int ret  = compress_block();
	int exp_ret = EXIT_FAILURE;
	fflush(stdout);

	if (ret == exp_ret) 
		return; // PASS

	if (fclose(fin) < 0 || fclose(fout) < 0){
		fprintf(stderr, "Couldn't close file");
		return;
	}

	sprintf(cmd, "%s bash -c 'cmp -n %d %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, BLOCKSIZE , in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// compress_block on non-ascii input
Test(compress_test_suite, compress_block_test3, .timeout = 5){
	char * in = "./tests/rsrc/c_emoji_test.in";
	char * out = "./test_output/compress_test_suite/c_emoji_test.out";

	FILE *fin = open_input_file(in);
	FILE *fout = open_output_file(out);
	global_options = (MAX_BLOCK_SIZE - 1) << 16;

	int ret  = compress_block();
	int exp_ret = EXIT_SUCCESS;
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);

	fflush(stdout);
	if (fclose(fin) < 0 || fclose(fout) < 0){
		fprintf(stderr, "Couldn't close file");
		return;
	}

	sprintf(cmd, "%s bash -c 'cmp -n %d %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS,  BLOCKSIZE, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// compress_block on input containing ascii and non-ascii text
Test(compress_test_suite, compress_block_test4, .timeout = 5){
	char * in = "./tests/rsrc/c_block_test4.in";
	char * out = "./test_output/compress_test_suite/c_block_test4.out";

	FILE* fin = open_input_file(in);
	FILE* fout = open_output_file(out);
	global_options = (4096 - 1) << 16;

	int ret  = compress_block();
	int exp_ret = EXIT_SUCCESS;
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);

	fflush(stdout);
	if (fclose(fin)< 0 || fclose(fout)< 0){
		fprintf(stderr, "Couldn't close file");
		return;
	}

	sprintf(cmd, "%s bash -c 'cmp -n %d %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS,  BLOCKSIZE, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// compress_block on one byte file
Test(compress_test_suite, compress_block_test5, .timeout = 5){
	char * in = "./tests/rsrc/c_single_byte_test.in";
	char * out = "./test_output/compress_test_suite/c_single_byte_test.out";

	FILE* fin = open_input_file(in);
	FILE* fout = open_output_file(out);
	global_options = (4096 - 1) << 16;

	int ret  = compress_block();
	int exp_ret = EXIT_SUCCESS;
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);

	fflush(stdout);
	if (fclose(fin)< 0 || fclose(fout)< 0){
		fprintf(stderr, "Couldn't close file");
		return;
	}

	sprintf(cmd, "%s bash -c 'cmp -n %d %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS,  BLOCKSIZE, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// compress_block on file with random bytes
Test(compress_test_suite, compress_block_test6, .timeout = 5){
	char * in = "./tests/rsrc/c_rand_3333.in";
	char * out = "./test_output/compress_test_suite/c_rand_3333.out";

	FILE* fin = open_input_file(in);
	FILE* fout = open_output_file(out);
	global_options = (4096 - 1) << 16;

	int ret  = compress_block();
	int exp_ret = EXIT_SUCCESS;
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);

	fflush(stdout);
	if (fclose(fin)< 0 || fclose(fout)< 0){
		fprintf(stderr, "Couldn't close file");
		return;
	}

	sprintf(cmd, "%s bash -c 'cmp -n %d %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS,  BLOCKSIZE, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// compress_block on file with random bytes
Test(compress_test_suite, compress_block_test7, .timeout = 5){
	char * in = "./tests/rsrc/c_rand_65536.in";
	char * out = "./test_output/compress_test_suite/c_rand_65536.out";

	FILE* fin = open_input_file(in);
	FILE* fout = open_output_file(out);
	global_options = (MAX_BLOCK_SIZE - 1) << 16;

	int ret  = compress_block();
	int exp_ret = EXIT_SUCCESS;
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);

	fflush(stdout);
	if (fclose(fin)< 0 || fclose(fout)< 0){
		fprintf(stderr, "Couldn't close file");
		return;
	}

	sprintf(cmd, "%s bash -c 'cmp -n %d %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS,  BLOCKSIZE, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// compress_block called multiple times on different inputs of varying sizes
Test(compress_test_suite, compress_block_1024_test, .timeout = 5){
	char * in = "./tests/rsrc/c_all_1024_test.in";
	char * out = "./test_output/compress_test_suite/c_all_1024_test.out";

	FILE *fin = open_input_file(in);
	FILE *fout = open_output_file(out);
	global_options = (1024 - 1) << 16;

	int ret  = compress_block();
	int exp_ret = EXIT_SUCCESS;
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);
	
	if (fclose(fin) < 0){
		fprintf(stderr, "Couldn't close file %s\n", in); return;
	}

	in = "./tests/rsrc/c_all_2048_test.in";
	fin = open_input_file(in);
	ret  = compress_block();
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);

	if (fclose(fin) < 0){
		fprintf(stderr, "Couldn't close file %s\n", in); return;
	}

	in = "./tests/rsrc/c_all_65536_test.in";
	fin = open_input_file(in);
	ret  = compress_block();
	cr_expect_eq(ret, exp_ret, "Invalid return for compress_block. Got %d | Expected: %d", ret, exp_ret);

	fflush(stdout);
	if (fclose(fin) < 0 || fclose(fout) < 0){
		fprintf(stderr, "Couldn't close file");
	}
}

// Test emit_huffman_tree on complete tree [0-255] 1024 bytes
Test(emit_huffman_tree_suite, emit_huffman_tree_test0, .timeout = 5) {
	char * in = "./tests/rsrc/e_tree_test0.in";
	char * out = "./test_output/emit_huffman_tree_suite/emit_tree_test0.out";

	FILE *fin = open_input_file(in);
	FILE *fout = open_output_file(out);

	test_read_huff_tree();
	emit_huffman_tree();

	fseek(fin, 0, 0);
	num_nodes = 0;
	num_nodes = fgetc(fin) << 8;
	num_nodes += fgetc(fin);

	close_output_file(fout);

	// sprintf(cmd, "%s cmp -n %d %s %s", STANDARD_LIMITS, HUFFTREEDESC, in, out);
	// int diff = WEXITSTATUS(system(cmd));
	// int exp_diff = 0;
	// cr_expect_eq(diff, exp_diff, "The output tree for emit huffman tree is not correct. Got %d | Expected: %d", diff, exp_diff);

	fseek(fin, 0, 0);
    if (read_huff_to_test_tree() != 0) {
        fprintf(stderr, "Error creating reference tree\n");
        return;
    }
	int nnodes = num_nodes;
	for(int i = 0; i < MAX_SYMBOLS; i++)
		*(node_for_symbol + i) = NULL;
	num_nodes = 0;
	
	int tree = check_nodes_tree_test(nodes);
	fprintf(stderr,"NUM OF SYMBOLS:	%d\n",num_of_symbols);

	cr_assert_eq(0, assert_isomorphism_test(), "Invalid Tree Structure, tree not Isomorphic with reference tree");
}

// Test emit_huffman_tree on tree only containing end symbol and one symbol
Test(emit_huffman_tree_suite, emit_huffman_tree_test1, .timeout = 5) {
	char * in = "./tests/rsrc/e_tree_test1.in";
	char * out = "./test_output/emit_huffman_tree_suite/emit_tree_test1.out";

	FILE *fin = open_input_file(in);
	FILE *fout = open_output_file(out);

	test_read_huff_tree();
	emit_huffman_tree();

	fseek(fin, 0, 0);
	num_nodes = 0;
	num_nodes = fgetc(fin) << 8;
	num_nodes += fgetc(fin);

	close_output_file(fout);

	// sprintf(cmd, "%s cmp -n %d %s %s", STANDARD_LIMITS, HUFFTREEDESC, in, out);
	// int diff = WEXITSTATUS(system(cmd));
	// int exp_diff = 0;
	// cr_expect_eq(diff, exp_diff, "The output tree for emit huffman tree is not correct. Got %d | Expected: %d", diff, exp_diff);


	fseek(fin, 0, 0);
    if (read_huff_to_test_tree() != 0) {
        fprintf(stderr, "Error creating reference tree\n");
        return;
    }
	int nnodes = num_nodes;
	for(int i = 0; i < MAX_SYMBOLS; i++)
		*(node_for_symbol + i) = NULL;
	num_nodes = 0;
	
	int tree = check_nodes_tree_test(nodes);
	fprintf(stderr,"NUM OF SYMBOLS:	%d\n",num_of_symbols);

	cr_assert_eq(0, assert_isomorphism_test(), "Invalid Tree Structure, tree not Isomorphic with reference tree");
}

// test on tree created from random input of 3333 bytes
Test(emit_huffman_tree_suite, emit_huffman_tree_test2, .timeout = 5) {
	char * in = "./tests/rsrc/e_tree_test2.in";
	char * out = "./test_output/emit_huffman_tree_suite/emit_tree_test2.out";

	FILE *fin = open_input_file(in);
	FILE *fout = open_output_file(out);

	test_read_huff_tree();
	emit_huffman_tree();
	fseek(fin, 0, 0);
	num_nodes = 0;
	num_nodes = fgetc(fin) << 8;
	num_nodes += fgetc(fin);

	close_output_file(fout);

	// sprintf(cmd, "%s cmp -n %d %s %s", STANDARD_LIMITS, HUFFTREEDESC, in, out);
	// int diff = WEXITSTATUS(system(cmd));
	// int exp_diff = 0;
	// cr_expect_eq(diff, exp_diff, "The output tree for emit huffman tree is not correct. Got %d | Expected: %d", diff, exp_diff);

	fseek(fin, 0, 0);
    if (read_huff_to_test_tree() != 0) {
        fprintf(stderr, "Error creating reference tree\n");
        return;
    }
	int nnodes = num_nodes;
	for(int i = 0; i < MAX_SYMBOLS; i++)
		*(node_for_symbol + i) = NULL;
	num_nodes = 0;
	
	int tree = check_nodes_tree_test(nodes);
	fprintf(stderr,"NUM OF SYMBOLS:	%d\n",num_of_symbols);

	cr_assert_eq(0, assert_isomorphism_test(), "Invalid Tree Structure, tree not Isomorphic with reference tree");
}

// test on tree created from random input of 65536 bytes
Test(emit_huffman_tree_suite, emit_huffman_tree_test3, .timeout = 5) {
	char * in = "./tests/rsrc/e_tree_test3.in";
	char * out = "./test_output/emit_huffman_tree_suite/emit_tree_test3.out";

	FILE *fin = open_input_file(in);
	FILE *fout = open_output_file(out);

	test_read_huff_tree();
	emit_huffman_tree();
	fseek(fin, 0, 0);
	num_nodes = 0;
	num_nodes = fgetc(fin) << 8;
	num_nodes += fgetc(fin);

	close_output_file(fout);

	// sprintf(cmd, "%s cmp -n %d %s %s", STANDARD_LIMITS, HUFFTREEDESC, in, out);
	// int diff = WEXITSTATUS(system(cmd));
	// int exp_diff = 0;
	// cr_expect_eq(diff, exp_diff, "The output tree for emit huffman tree is not correct. Got %d | Expected: %d", diff, exp_diff);

	fseek(fin, 0, 0);
    if (read_huff_to_test_tree() != 0) {
        fprintf(stderr, "Error creating reference tree\n");
        return;
    }
	int nnodes = num_nodes;
	for(int i = 0; i < MAX_SYMBOLS; i++)
		*(node_for_symbol + i) = NULL;
	num_nodes = 0;
	
	int tree = check_nodes_tree_test(nodes);
	fprintf(stderr,"NUM OF SYMBOLS:	%d\n",num_of_symbols);

	cr_assert_eq(0, assert_isomorphism_test(), "Invalid Tree Structure, tree not Isomorphic with reference tree");
}


int assert_isomorphism_test(){
	int i;
	NODE * np;
	NODE * t_np;
	
	// Iterate through all the symbols found in the student tree
	for (i = 0; i < num_of_symbols; i++){
		// fprintf(stderr,"%hu\n",symbols[i]);
		np = *(node_for_symbol + symbols[i]);
		t_np = *(t_node_for_symbol + symbols[i]);
		// Error If symbol does not exist in reference tree
		if (t_np == NULL){
			return 1;
		}
		while (np != nodes && t_np != t_nodes){
			// Ensure student parent pointers are in bounds
			if (np->parent < nodes || np->parent >= (nodes + 2 * MAX_SYMBOLS - 1))
				return 1;
			np = np->parent;
			t_np = t_np->parent;
		}
		// Check if both have reached the root of their respective tree
		if (np - nodes != t_np - t_nodes)
			return 1;
	}
	return 0;
}


int check_nodes_tree_test(NODE * node){
	if (node == NULL)
		return 1;
	if (node->left == NULL && node->right == NULL){
		if (((node->symbol & 0xff00) > 0)) // If End Symbol make it same end Symbol as ref tree, for ease of indexing
			node->symbol = 0x100;
		if (*(node_for_symbol + node->symbol) != NULL)
			return 1;
		
		*(node_for_symbol + node->symbol) = node;
		symbols[num_of_symbols] = node->symbol;
		num_of_symbols++;
	} else{
		if (node->left != NULL) {
			if (node->left >= (nodes + 2 * MAX_SYMBOLS - 1) || node->left < nodes) 
				return 1;
			if (check_nodes_tree_test(node->left) == 1)
				return 1; 
		}
		if (node->right != NULL) {
			if (node->right >= (nodes + 2 * MAX_SYMBOLS - 1) || node->right < nodes) 
				return 1;
			if (check_nodes_tree_test(node->right) == 1)
				return 1;
		}
	}
	num_nodes++;
	return 0;
}