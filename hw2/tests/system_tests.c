#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "test_common.h"
#include "global.h"

// TODO: This really ought to be dynamically allocated.
static char cmd[512];

// Test compress when used with pipes
Test(compress_system_suite, compress_pipe_test, .timeout = 5){
	char * in = "./tests/rsrc/c_test0.in";
	char * out = "./test_output/compress_system_suite/c_pipe_test.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s cat %s | bin/huff -c > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test compress on gettysburg.txt in Two Separate Blocks
Test(compress_system_suite, compress_test0, .timeout = 5){
	char * in = "./tests/rsrc/c_test0.in";
	char * out = "./test_output/compress_system_suite/c_test0.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c -b 1024 < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test input with size less than blocksize
Test(compress_system_suite, compress_test1, .timeout = 5){
	char * in = "./tests/rsrc/c_block_test0.in";
	char * out = "./test_output/compress_system_suite/c_test1.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test input with all symbols
Test(compress_system_suite, compress_test2, .timeout = 5){
	char * in = "./tests/rsrc/c_all_1024_test.in";
	char * out = "./test_output/compress_system_suite/c_test2.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test on input with size larger than blocksize
Test(compress_system_suite, compress_test3, .timeout = 5){
	char * in = "./tests/rsrc/c_rand_3333.in";
	char * out = "./test_output/compress_system_suite/c_test3.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c -b 1024 < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test on random input of size 1024
Test(compress_system_suite, compress_test4, .timeout = 5){
	char * in = "./tests/rsrc/c_rand_1024.in";
	char * out = "./test_output/compress_system_suite/c_test4.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test on empty
Test(compress_system_suite, compress_empty, .timeout = 5){
	char * in = "./tests/rsrc/c_empty_test.in";
	char * out = "./test_output/compress_system_suite/c_test5.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test on single byte
Test(compress_system_suite, compress_single_byte, .timeout = 5){
	char * in = "./tests/rsrc/c_single_byte_test.in";
	char * out = "./test_output/compress_system_suite/c_test6.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test on non-ascii text
Test(compress_system_suite, compress_emoji, .timeout = 5){
	char * in = "./tests/rsrc/c_emoji_test.in";
	char * out = "./test_output/compress_system_suite/c_test7.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test on ascii and non-ascii text
Test(compress_system_suite, compress_emoji_ascii, .timeout = 5){
	char * in = "./tests/rsrc/c_block_test4.in";
	char * out = "./test_output/compress_system_suite/c_test8.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

// Test on file much larger than blocksize
Test(compress_system_suite, compress_large, .timeout = 5){
	char * in = "./tests/rsrc/c_rand_65536.in";
	char * out = "./test_output/compress_system_suite/c_large.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s bin/huff -c -b 2000 < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, in, out);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

////////////////////////////
// Decompress Tests
////////////////////////////

Test(decompress_system_suite, decompress_pipe_test, .timeout = 5){
	char * in = "./tests/rsrc/de_test0.in";
	char * out = "./test_output/decompress_system_suite/decompress_pipe_test.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s cat %s | bin/huff -d > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, out, in);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

Test(decompress_system_suite, decompress_test0, .timeout = 5){
	char * in = "./tests/rsrc/de_test0.in";
	char * out = "./test_output/decompress_system_suite/decompress_test0.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, out, in);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

Test(decompress_system_suite, decompress_test1, .timeout = 5){
	char * in = "./tests/rsrc/d_all_1024_test.in";
	char * out = "./test_output/decompress_system_suite/decompress_test1.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, out, in);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}


Test(decompress_system_suite, decompress_test2, .timeout = 5){
	char * in = "./tests/rsrc/d_emoji_test.in";
	char * out = "./test_output/decompress_system_suite/decompress_test2.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, out, in);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

Test(decompress_system_suite, decompress_test3, .timeout = 5){
	char * in = "./tests/rsrc/d_block_test0.in";
	char * out = "./test_output/decompress_system_suite/decompress_test3.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, out, in);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

Test(decompress_system_suite, decompress_test4, .timeout = 5){
	char * in = "./tests/rsrc/d_rand_3333.in";
	char * out = "./test_output/decompress_system_suite/decompress_test4.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, out, in);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

Test(decompress_system_suite, decompress_test5, .timeout = 5){
	char * in = "./tests/rsrc/d_rand_65536.in";
	char * out = "./test_output/decompress_system_suite/decompress_test5.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, out, in);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

Test(decompress_system_suite, decompress_test6, .timeout = 5){
	char * in = "./tests/rsrc/d_emoji_ascii.in";
	char * out = "./test_output/decompress_system_suite/decompress_test6.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	sprintf(cmd, "%s bash -c 'cmp %s <(cat %s | ./tests/ref_huff -d)'", STANDARD_LIMITS, out, in);
	int diff = WEXITSTATUS(system(cmd));
	int exp_diff = 0;
	cr_expect_eq(diff, exp_diff, "The output when decompressed is not equivalent to the original file. Got %d | Expected: %d", diff, exp_diff);
}

Test(decompress_system_suite, decompress_no_end_block, .timeout = 5){
	char * in = "./tests/rsrc/d_no_end_block.in";
	char * out = "./test_output/decompress_system_suite/decompress_block_neb.out";
	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

Test(decompress_system_suite, decompress_no_end_symbol, .timeout = 5){
	char * in = "./tests/rsrc/d_no_end_symbol.in";
	char * out = "./test_output/decompress_system_suite/decompress_block_nes.out";
	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

Test(decompress_system_suite, decompress_no_leaf_nodes, .timeout = 5){
	char * in = "./tests/rsrc/d_not_enough_leaf_nodes.in";
	char * out = "./test_output/decompress_system_suite/decompress_block_nln.out";

	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

Test(decompress_system_suite, decompress_no_internal_nodes, .timeout = 5){
	char * in = "./tests/rsrc/d_not_enough_internal_nodes.in";
	char * out = "./test_output/decompress_system_suite/decompress_block_nin.out";
	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

Test(decompress_system_suite, decompress_invalid_num_nodes, .timeout = 5){
	char * in = "./tests/rsrc/d_invalid_num_nodes0.in";
	char * out = "./test_output/decompress_system_suite/decompress_block_inn.out";
	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	// in = "./tests/rsrc/d_invalid_num_nodes1.in";
	// sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	// ret = WEXITSTATUS(system(cmd));
	// cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);

	// in = "./tests/rsrc/d_invalid_num_nodes2.in";
	// sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	// ret = WEXITSTATUS(system(cmd));
	// cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

Test(decompress_system_suite, decompress_trunc_at_traversal, .timeout = 5){
	char * in = "./tests/rsrc/d_trunc_at_traversal.in";
	char * out = "./test_output/decompress_system_suite/decompress_block_tat.out";

	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

Test(decompress_system_suite, decompress_empty, .timeout = 5){
	char * in = "./tests/rsrc/d_empty.in";
	char * out = "./test_output/decompress_system_suite/decompress_block_empty.out";
	int exp_ret = EXIT_SUCCESS;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

Test(decompress_system_suite, decompress_single_byte, .timeout = 5){
	char * in = "./tests/rsrc/d_single_byte.in";
	char * out = "./test_output/decompress_system_suite/decompress_block_single_byte.out";
	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

Test(decompress_system_suite, decompress_trunc_at_symbols, .timeout = 5){
	char * in = "./tests/rsrc/d_trunc_at_symbols.in";
	char * out = "./test_output/decompress_system_suite/decompress_block_tas.out";

	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

// Test compress when there is a memory limit
// Results in file size limit exceeded (core dumped)
// I'm not sure how it would be handled so not used
/*
Test(decompress_system_suite, decompress_mem_test, .timeout = 5){
	char * in = "./tests/rsrc/d_rand_3333.in";
	char * out = "./tests/rsrc/d_mem_test.out";
	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s ulimit -f 1 &&  bin/huff -d < %s > %s", STANDARD_LIMITS, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for decompress. Got %d | Expected: %d", ret, exp_ret);
}

Test(compress_system_suite, compress_mem_test, .timeout = 5){
	char * in = "./tests/rsrc/c_rand_3333.in";
	char * out = "./tests/rsrc/c_mem_test.out";

	int blocksize = MAX_BLOCK_SIZE;
	int exp_ret = EXIT_FAILURE;

	sprintf(cmd, "%s ulimit -f 1 &&  bin/huff -c -b %d < %s > %s", STANDARD_LIMITS, blocksize, in, out);
	int ret = WEXITSTATUS(system(cmd));
	cr_expect_eq(ret, exp_ret, "Invalid return for compress. Got %d | Expected: %d", ret, exp_ret);
}
*/
