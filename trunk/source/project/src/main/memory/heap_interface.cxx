#include <heap_interface.h>
#include <log.h>

static void dump_info(const char* title, int value) {
	Core::Util::DebugPrintf("%s : ", title);

	if (value == -1) {
		Core::Util::DebugPrintf("Unknown\n");
	}
	else {
		Core::Util::DebugPrintf("%d.\n", value);
	}
}

void heap_dump_helper(
		int tot_alloc_number, 
		int tot_alloc_size, 
		int tot_free_number, 
		int tot_free_size, 
		int largest_free_size) {

	dump_info("Total allocated memory block number is", tot_alloc_number);
	dump_info("Total allocated memory size is", tot_alloc_size);
	dump_info("Total free memory block number is", tot_free_number);
	dump_info("Total free memory size is", tot_free_size);
	dump_info("Largest free memory block size is", largest_free_size);

	return;
}
