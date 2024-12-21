#include <criterion/criterion.h>
#include <criterion/logging.h>
#include "global.h"

////////////////////////////////////////////
// COMPRESS TEST
////////////////////////////////////////////
Test(validargs_test_suite, validargs_default_compress_test, .timeout = 5){
	int argc = 2;
	char *argv[] = {"bin/huff", "-c", NULL};
	int ret = validargs(argc, argv);
	int exp_ret = 0;
	unsigned long opt = global_options;
	unsigned long flag = 0xffff << 16;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & flag, flag, "Incorrect block size set. Got %d | Expected: %d", opt >> 16, flag >> 16);

	flag = 0x2;
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);
}

Test(validargs_test_suite, validargs_compress_test0, .timeout = 5){
	int argc = 4;
	char *argv0[] = {"bin/huff", "-c", "-b", "1024", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = 0;
	unsigned long opt = global_options;
	unsigned long flag = 1023 << 16;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & flag, flag, "Incorrect block size set. Got %d | Expected: %d", opt >> 16, flag >> 16);

	flag = 0x2;
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);
}

Test(validargs_test_suite, validargs_compress_test1, .timeout = 5){
	int argc = 4;
	char *argv0[] = {"bin/huff", "-c", "-b", "65536", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = 0;
	unsigned long opt = global_options;
	unsigned long flag = 65535 << 16;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & flag, flag, "Incorrect block size set. Got %d | Expected: %d", opt >> 16, flag >> 16);

	flag = 0x2;
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);
}

Test(validargs_test_suite, validargs_compress_test2, .timeout = 5){
	int argc = 4;
	char *argv0[] = {"bin/huff", "-c", "-b", "00002048", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = 0;
	unsigned long opt = global_options;
	unsigned long flag = 2047 << 16;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & flag, flag, "Incorrect block size set. Got %d | Expected: %d", opt >> 16, flag >> 16);

	flag = 0x2;
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);
}

Test(validargs_test_suite, validargs_compress_invalid_block_test0, .timeout = 5){
	int argc = 3;
	char *argv0[] = {"bin/huff", "-c", "-b", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = -1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	argc = 4;
	global_options = 0;
	char *argv1[] = {"bin/huff", "-c", "-b", "1023", NULL};
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0;
	argv1[3] = "0";
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0;
	argv1[3] = "65537";
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0;
	argv1[3] = "-2048";
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
}

Test(validargs_test_suite, validargs_compress_invalid_block_test1, .timeout = 5){
	int argc = 4;
	char *argv1[] = {"bin/huff", "-c", "-b", "wrong", NULL};
	int ret = validargs(argc, argv1);
	int exp_ret = -1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0;
	argv1[3] = "8332T";
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0;
	argv1[3] = "\n!./|";
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
}

Test(validargs_test_suite, validargs_invalid_compress_test, .timeout = 5){
	int argc = 3;
	char *argv[] = {"bin/huff", "CSE320", "-c", NULL};
	int ret = validargs(argc, argv);
	int exp_ret = -1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argv[3] = argv[1]; argv[1] = argv[2]; argv[2] = argv[3]; argv[3] = NULL;
	ret = validargs(argc, argv);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
}

////////////////////////////////////////////
// DECOMPRESS TEST
////////////////////////////////////////////
Test(validargs_test_suite, validargs_decompress_test, .timeout = 5){
	int argc = 2;
	char *argv[] = {"bin/huff", "-d", NULL};
	int ret = validargs(argc, argv);
	int exp_ret = 0;
	unsigned long opt = global_options;
	unsigned long flag = 0x4;
	cr_assert_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);
}

// Commands that are otherwise correct aside from the order
Test(validargs_test_suite, validargs_invalid_order_test, .timeout = 5){
	int exp_ret = -1;

	int argc = 3;
	char *argv[] = {"bin/huff", "-c", "-h", NULL};
	int ret = validargs(argc, argv);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argv[1] = "-d";
	ret = validargs(argc, argv);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	
	global_options = 0L;
	argc = 4;
	char *argv0[] = {"bin/huff", "-b", "1024", "-c", NULL};
	ret = validargs(argc, argv0);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argc = 5;
	char *argv1[] = {"bin/huff", "-c", "-b", "1024", "-h", NULL};
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
}

// Commands that involve an incorrect combination of flags/arguments
Test(validargs_test_suite, validargs_invalid_combination_test0, .timeout = 5){
	int exp_ret = -1;

	int argc = 3;
	char *argv0[] = {"bin/huff", "-c", "-d", NULL};
	int ret = validargs(argc, argv0);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	char *argv1[] = {"bin/huff", "-d", "-c", NULL};
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argc = 4;
	char *argv2[] = {"bin/huff", "-d", "-b", "1024", NULL};
	ret = validargs(argc, argv2);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argc = 5;
	char *argv3[] = {"bin/huff", "-c", "-b", "1024", "-d", NULL};
	ret = validargs(argc, argv3);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
}

////////////////////////////////////////////
// HELP TEST
////////////////////////////////////////////
Test(validargs_test_suite, validargs_help_test0, .timeout = 5){
	int argc = 2;
	char *argv0[] = {"bin/huff", "-h", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = 0;
	unsigned long opt = global_options;
	unsigned long flag = 0x1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);
}

Test(validargs_test_suite, validargs_help_test1, .timeout = 5){
	int argc = 3;
	char *argv0[] = {"bin/huff", "-h", "-c", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = 0;
	unsigned long opt = global_options;
	unsigned long flag = 0x1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);

	global_options = 0L;
	argv0[2] = "-d";
	ret = validargs(argc, argv0);
	opt = global_options;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);

	global_options = 0L;
	argv0[2] = "-b";
	ret = validargs(argc, argv0);
	opt = global_options;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);
}

Test(validargs_test_suite, validargs_help_test2, .timeout = 5){
	int argc = 4;
	char *argv0[] = {"bin/huff", "-h", "HELP", "ME", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = 0;
	unsigned long opt = global_options;
	unsigned long flag = 0x1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);
}

Test(validargs_test_suite, validargs_help_test3, .timeout = 5){
	int argc = 4;
	char *argv0[] = {"bin/huff", "-h", "-h", "ME", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = 0;
	unsigned long opt = global_options;
	unsigned long flag = 0x1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
	cr_expect_eq(opt & 0x7, flag, "Invalid options set. Got %d | Expected: %d", opt & 0x7, flag);
}

// GENERIC TEST
Test(validargs_test_suite, validargs_invalid_test0, .timeout = 5){
	int argc = 3;
	char *argv0[] = {"bin/huff", "-w", "-h", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = -1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argv0[1] = "-H";
	ret = validargs(argc, argv0);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argv0[1] = "123908";
	ret = validargs(argc, argv0);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argv0[1] = "incorrect";
	ret = validargs(argc, argv0);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	argc = 2;
	global_options = 0L;
	char *argv1[] = {"bin/huff", "--h", NULL};
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
}

Test(validargs_test_suite, validargs_invalid_test1, .timeout = 5){
	int argc = 1;
	char *argv0[] = {"bin/huff", NULL};
	int ret = validargs(argc, argv0);
	int exp_ret = -1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
}

Test(validargs_test_suite, validargs_invalid_test2, .timeout = 5){
	int argc = 2;
	char *argv1[] = {"bin/huff", "-compress", NULL};
	int ret = validargs(argc, argv1);
	int exp_ret = -1;
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argv1[1] = "-decompress";
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	global_options = 0L;
	argv1[1] = "-hhhelp";
	ret = validargs(argc, argv1);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);

	argc = 3;
	global_options = 0L;
	char *argv2[] = {"bin/huff", "-compress", "-b", "2046", NULL};
	ret = validargs(argc, argv2);
	cr_expect_eq(ret, exp_ret, "Invalid return for valid args. Got %d | Expected: %d", ret, exp_ret);
}

