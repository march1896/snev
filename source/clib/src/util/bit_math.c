#include <cominc.h>
#include <bit_math.h>

inline int mlog2(int x) {
	int bit = 0;
	if (x & 0xFFFF0000) {
		x = x >> 16;
		bit += 16;
	}

	if (x & 0xFF00) {
		x = x >> 8;
		bit += 8;
	}

	if (x & 0xF0) {
		x = x >> 4;
		bit += 4;
	}

	if (x & 0xC) {
		x = x >> 2;
		bit += 2;
	}

	if (x & 0x2) {
		x = x >> 1;
		bit += 1;
	}

	return bit;
}
