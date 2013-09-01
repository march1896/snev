#include <llrb_link.h>
#include <stdlib.h>
#include <stdio.h>
#include "test_util.h"

typedef struct llrb_link tree_link;
#define tree_insert llrb_insert
#define tree_insert_s llrb_insert_s
#define tree_insert_v llrb_insert_v
#define tree_insert_sv llrb_insert_sv
#define tree_remove llrb_remove
#define tree_remove_v llrb_remove_v
#define tree_search llrb_search
#define pf_tree_compare pf_llrb_compare
#define tree_min llrb_min
#define tree_max llrb_max
#define tree_check llrb_debug_check
#define tree_check_v llrb_debug_check_v

#include "tree_link.util.h"

void llrb_link_test() {
	test_run_single("llrb link correctness test", tree_link_correctness_test);
	test_run_bench("llrb link performance test", tree_link_performance_test);
}

#undef tree_insert
#undef tree_insert_s
#undef tree_insert_v
#undef tree_insert_sv
#undef tree_remove
#undef tree_remove_v
#undef tree_search
#undef pf_tree_compare
#undef tree_min
#undef tree_max
#undef tree_check
#undef tree_check_v
