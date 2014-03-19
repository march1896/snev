#include <memheap/heap_def.h>
#include <memheap/heap_walk.h>

#include <util/math.h>

#include <stdio.h>
#include <stdarg.h>

// void log_printf(const char* format, ...) {
// 	va_list arglist;
// 	va_start( arglist, format );
// 	vprintf(format, arglist);
// 	if (g_use_file) {
// 		FILE * pFile = fopen(g_log_file, "a");
// 		vfprintf(pFile, format, arglist);
// 		fclose(pFile);
// 	}
// 	va_end( arglist );
// }

void heap_leak_print_to_terminal(struct heap_blockinfo* binfo, void* param) {
	unused(param);
	if (binfo->allocated) {
		printf("0X%08X", (int)(intptr_t)binfo->allocable_addr);
		printf("%8d bytes", binfo->allocable_size);
		printf(" at %s %d\n", binfo->file, binfo->line);
	}
}

void heap_state_init(struct heap_state* stat) {
	stat->total_allocable = 0;
	stat->total_inuse = 0;
	stat->max_allocable = 0;
}

void heap_query_state(struct heap_blockinfo* binfo, void* param) {
	struct heap_state* stat = (struct heap_state*)param;

	if (binfo->allocated) {
		stat->total_inuse += binfo->allocable_size;
	}
	else {
		stat->total_allocable += binfo->allocable_size;
		stat->max_allocable = max(stat->max_allocable, binfo->allocable_size);
	}
}
