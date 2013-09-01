#include <splay_link.h>
#include <stdlib.h>
#include <stdio.h>
#include "test_util.h"

typedef struct splay_link tree_link;
#define tree_insert splay_insert
#define tree_insert_s splay_insert_s
#define tree_insert_v splay_insert_v
#define tree_insert_sv splay_insert_sv
#define tree_remove splay_remove
#define tree_remove_v splay_remove_v
#define tree_search splay_search
#define tree_dynamic_search splay_dynamic_search
#define pf_tree_compare pf_splay_compare
#define tree_min splay_min
#define tree_max splay_max
#define tree_check splay_debug_check
#define tree_check_v splay_debug_check_v

#include "tree_link.util.h"

void splay_link_test() {
	test_run_single("splay link correctness test", tree_link_correctness_test);
	test_run_bench("splay link performance test", tree_link_performance_test);
}

#undef tree_insert
#undef tree_insert_s
#undef tree_insert_v
#undef tree_insert_sv
#undef tree_remove
#undef tree_remove_v
#undef tree_search
#undef tree_dynamic_search
#undef pf_tree_compare
#undef tree_min
#undef tree_max
#undef tree_check
#undef tree_check_v
